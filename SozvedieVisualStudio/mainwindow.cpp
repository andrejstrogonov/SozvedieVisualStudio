#include "mainwindow.h"
#include "numericdelegate.h"
#include <QtSql>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QPair>
#include <QStandardItemModel>
#include <QStandardItem>

MainWindow::MainWindow()
{
    currentTransmitter = -1;

    createUI();
    loadTree();
}

void MainWindow::createUI()
{
    QWidget* central = new QWidget;
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout;

    // Дерево объектов (QTreeView + QStandardItemModel)
    treeView = new QTreeView;
    treeModel = new QStandardItemModel;
    treeView->setModel(treeModel);
    treeView->setItemDelegate(new TreeModelDelegate(this));
    treeView->setHeaderHidden(true);

    // Таблица ТТХ (QTableView + QSqlTableModel)
    tableView = new QTableView;
    specModel = new QSqlTableModel;
    tableView->setModel(specModel);
    tableView->setItemDelegate(new NumericDelegate(this));

    // Кнопки
    addObjectBtn = new QPushButton("Add Object");
    addTransmitterBtn = new QPushButton("Add Transmitter");
    deleteBtn = new QPushButton("Delete");

    connect(addObjectBtn, &QPushButton::clicked, this, &MainWindow::addObject);
    connect(addTransmitterBtn, &QPushButton::clicked, this, &MainWindow::addTransmitter);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteElement);

    QVBoxLayout* left = new QVBoxLayout;
    left->addWidget(treeView);
    left->addWidget(addObjectBtn);
    left->addWidget(addTransmitterBtn);
    left->addWidget(deleteBtn);

    QVBoxLayout* right = new QVBoxLayout;
    right->addWidget(tableView);

    mainLayout->addLayout(left);
    mainLayout->addLayout(right);

    central->setLayout(mainLayout);

    resize(1200, 700);
}

void MainWindow::loadTree()
{
    treeModel->clear();
    treeModel->setColumnCount(2);
    treeModel->setHeaderData(0, Qt::Horizontal, "Имя");
    treeModel->setHeaderData(1, Qt::Horizontal, "Координаты");

    QSqlQuery query("SELECT id, name, latitude, longitude FROM objects");
    while (query.next())
    {
        int objID = query.value(0).toInt();
        QString name = query.value(1).toString();
        double lat = query.value(2).toDouble();
        double lon = query.value(3).toDouble();

        QStandardItem* objItem = new QStandardItem(name);
        objItem->setData(QVariant::fromValue(qMakePair(QString("object"), objID)), Qt::UserRole);
        objItem->setData(QString("(%1, %2)").arg(lat).arg(lon), Qt::DisplayRole);

        QStandardItem* objNode = new QStandardItem("Объект");
        objNode->setParent(objItem);
        objNode->setFlags(objNode->flags() & ~Qt::ItemIsEditable);

        QSqlQuery tx;
        tx.prepare("SELECT id, name FROM transmitters WHERE object_id = :id");
        tx.bindValue(":id", objID);
        tx.exec();

        while (tx.next())
        {
            int txID = tx.value(0).toInt();
            QString txName = tx.value(1).toString();

            QStandardItem* txItem = new QStandardItem(txName);
            txItem->setData(QVariant::fromValue(qMakePair(QString("transmitter"), txID)), Qt::UserRole);
            txItem->setData(QString("Передатчик %1").arg(txID), Qt::DisplayRole);

            QStandardItem* txNode = new QStandardItem("ТТХ");
            txNode->setParent(txItem);
            txNode->setFlags(txNode->flags() & ~Qt::ItemIsEditable);

            treeModel->appendRow(objItem);
        }
    }
}

void MainWindow::onTreeChanged(const QModelIndex& index)
{
    if (!index.isValid())
    {
        tableView->setRowCount(0);
        currentTransmitter = -1;
        return;
    }

    QVariant var = index.data(Qt::UserRole);
    if (!var.canConvert<QPair<QString, int>>())
    {
        tableView->setRowCount(0);
        currentTransmitter = -0;
        return;
    }

    auto pair = var.value<QPair<QString, int>>();
    QString type = pair.first;
    int id = pair.second;

    if (type == "transmitter" || type == "specs")
    {
        currentTransmitter = id;
        loadSpecs(id);
    }
    else
    {
        tableView->setRowCount(0);
        currentTransmitter = -1;
    }
}

void MainWindow::onTableChanged(const QModelIndex& index)
{
    if (!index.isValid() || currentTransmitter == -1)
        return;

    QString paramName = specModel->headerData(index.row(), Qt::Horizontal).toString();
    bool ok;
    double value = specModel->data(index).toDouble(&ok);

    if (!ok)
    {
        QMessageBox::warning(this, "Ошибка", "Введите корректное числовое значение");
        loadSpecs(currentTransmitter);
        return;
    }

    QString field;
    if (paramName == "Мощность (Вт)")
        field = "power_watt";
    else if (paramName == "КУ (дБ)")
        field = "gain_db";
    else if (paramName == "Высота подвеса (м)")
        field = "antenna_height";
    else
        return;

    QSqlQuery query;
    query.prepare(QString("UPDATE specs SET %1 = :value WHERE transmitter_id = :id").arg(field));
    query.bindValue(":value", value);
    query.bindValue(":id", currentTransmitter);

    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось обновить данные: " + query.lastError().text());
        loadSpecs(currentTransmitter);
    }
}

void MainWindow::addObject()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Добавить объект", "Введите имя объекта:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty())
        return;

    double lat = QInputDialog::getDouble(this, "Координаты", "Широта:", 0, -90, 90, 4, &ok);
    if (!ok)
        return;
    double lon = QInputDialog::getDouble(this, "Координаты", "Долгота:", 0, -180, 180, 4, &ok);
    if (!ok)
        return;

    QSqlQuery query;
    query.prepare("INSERT INTO objects (name, latitude, longitude) VALUES (:name, :lat, :lon)");
    query.bindValue(":name", name);
    query.bindValue(":lat", lat);
    query.bindValue(":lon", lon);

    if (query.exec())
    {
        loadTree();
    }
    else
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось добавить объект: " + query.lastError().text());
    }
}

void MainWindow::addTransmitter()
{
    QList<QModelIndex> selected = treeView->selectedIndexes();
    if (selected.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Выберите объект для добавления передатчика");
        return;
    }

    QModelIndex index = selected.first();
    QVariant var = index.data(Qt::UserRole);
    if (!var.canConvert<QPair<QString, int>>())
    {
        QMessageBox::warning(this, "Ошибка", "Выберите объект (не передатчик)");
        return;
    }

    auto pair = var.value<QPair<QString, int>>();
    if (pair.first != "object")
    {
        QMessageBox::warning(this, "Ошибка", "Выберите объект (не передатчик)");
        return;
    }

    int objectId = pair.second;

    bool ok;
    QString name = QInputDialog::getText(this, "Добавить передатчик", "Введите имя передатчика:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty())
        return;

    QSqlDatabase::database().transaction();

    QSqlQuery query;
    query.prepare("INSERT INTO transmitters (object_id, name) VALUES (:obj, :name)");
    query.bindValue(":obj", objectId);
    query.bindValue(":name", name);

    if (query.exec())
    {
        int txId = query.lastInsertId().toInt();

        QSqlQuery specQuery;
        specQuery.prepare("INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (:id, 0.0, 0.0, 0.0)");
        specQuery.bindValue(":id", txId);

        if (specQuery.exec())
        {
            QSqlDatabase::database().commit();
            loadTree();
        }
        else
        {
            QSqlDatabase::database().rollback();
            QMessageBox::critical(this, "Ошибка", "Не удалось создать ТТХ для передатчика: " + specQuery.lastError().text());
        }
    }
    else
    {
        QSqlDatabase::database().rollback();
        QMessageBox::critical(this, "Ошибка", "Не удалось добавить передатчик: " + query.lastError().text());
    }
}

void MainWindow::deleteElement()
{
    QList<QModelIndex> selected = treeView->selectedIndexes();
    if (selected.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Выберите элемент для удаления");
        return;
    }

    QModelIndex index = selected.first();
    QVariant var = index.data(Qt::UserRole);
    if (!var.canConvert<QPair<QString, int>>())
        return;

    auto pair = var.value<QPair<QString, int>>();
    QString type = pair.first;
    int id = pair.second;

    QString message;
    if (type == "object")
        message = "Вы уверены, что хотите удалить объект и все его передатчики?";
    else if (type == "transmitter")
        message = "Вы уверены, что хотите удалить этот передатчик?";
    else
        return;

    int reply = QMessageBox::question(this, "Подтверждение", message, QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;

    QSqlQuery query;
    if (type == "object")
        query.prepare("DELETE FROM objects WHERE id = :id");
    else // transmitter
        query.prepare("DELETE FROM transmitters WHERE id = :id");

    query.bindValue(":id", id);

    if (query.exec())
    {
        loadTree();
        tableView->setRowCount(0);
        currentTransmitter = -1;
    }
    else
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось удалить элемент: " + query.lastError().text());
    }
}

void MainWindow::loadSpecs(int transmitterID)
{
    specModel->setTable("specs");
    specModel->setFilter(QString("transmitter_id = %1").arg(transmitterID));
    specModel->select();

    if (specModel->rowCount() > 0)
    {
        tableView->setColumnCount(2);
        tableView->setHorizontalHeaderLabels({ "Параметр", "Значение" });
    }
    else
    {
        tableView->setColumnCount(0);
    }
}
