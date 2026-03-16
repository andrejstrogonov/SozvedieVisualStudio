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

    tree = new QTreeWidget;
    tree->setHeaderLabels({ "Objects","Coordinates" });

    connect(tree, &QTreeWidget::itemSelectionChanged,
        this, &MainWindow::onTreeChanged);

    table = new QTableWidget(0, 2);
    table->setHorizontalHeaderLabels({ "Parameter","Value" });
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setItemDelegate(new NumericDelegate(this));

    connect(table, &QTableWidget::itemChanged,
        this, &MainWindow::onTableChanged);

    addObjectBtn = new QPushButton("Add Object");
    addTransmitterBtn = new QPushButton("Add Transmitter");
    deleteBtn = new QPushButton("Delete");

    connect(addObjectBtn, &QPushButton::clicked, this, &MainWindow::addObject);
    connect(addTransmitterBtn, &QPushButton::clicked, this, &MainWindow::addTransmitter);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteElement);

    QVBoxLayout* left = new QVBoxLayout;
    left->addWidget(tree);
    left->addWidget(addObjectBtn);
    left->addWidget(addTransmitterBtn);
    left->addWidget(deleteBtn);

    QVBoxLayout* right = new QVBoxLayout;
    right->addWidget(table);

    mainLayout->addLayout(left);
    mainLayout->addLayout(right);

    central->setLayout(mainLayout);

    resize(1200, 700);
}

void MainWindow::loadTree()
{
    tree->clear();

    QSqlQuery query("SELECT id, name, latitude, longitude FROM objects");
    while (query.next())
    {
        int objID = query.value(0).toInt();
        QString name = query.value(1).toString();
        double lat = query.value(2).toDouble();
        double lon = query.value(3).toDouble();

        QTreeWidgetItem* obj = new QTreeWidgetItem(tree);
        obj->setText(0, name);
        obj->setText(1, QString("(%1, %2)").arg(lat).arg(lon));
        obj->setData(0, Qt::UserRole, QVariant::fromValue(qMakePair(QString("object"), objID)));

        QSqlQuery tx;
        tx.prepare("SELECT id, name FROM transmitters WHERE object_id = :id");
        tx.bindValue(":id", objID);
        tx.exec();

        while (tx.next())
        {
            int txID = tx.value(0).toInt();
            QString txName = tx.value(1).toString();

            QTreeWidgetItem* t = new QTreeWidgetItem(obj);
            t->setText(0, txName);
            t->setData(0, Qt::UserRole, QVariant::fromValue(qMakePair(QString("transmitter"), txID)));

            QTreeWidgetItem* specsNode = new QTreeWidgetItem(t);
            specsNode->setText(0, "ТТХ");
            specsNode->setData(0, Qt::UserRole, QVariant::fromValue(qMakePair(QString("specs"), txID)));
        }
    }
}

void MainWindow::onTreeChanged()
{
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.isEmpty())
    {
        table->setRowCount(0);
        currentTransmitter = -1;
        return;
    }

    QTreeWidgetItem* item = selected.first();
    QVariant var = item->data(0, Qt::UserRole);
    if (!var.canConvert<QPair<QString,int>>())
    {
        table->setRowCount(0);
        currentTransmitter = -1;
        return;
    }

    auto pair = var.value<QPair<QString,int>>();
    QString type = pair.first;
    int id = pair.second;

    if (type == "transmitter" || type == "specs")
    {
        currentTransmitter = id;
        loadSpecs(id);
    }
    else
    {
        table->setRowCount(0);
        currentTransmitter = -1;
    }
}

void MainWindow::onTableChanged(QTableWidgetItem* item)
{
    if (!item || item->column() != 1 || currentTransmitter == -1)
        return;

    int row = item->row();
    QString paramName = table->item(row, 0)->text();
    bool ok;
    double value = item->text().toDouble(&ok);
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
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Выберите объект для добавления передатчика");
        return;
    }

    QTreeWidgetItem* item = selected.first();
    QVariant var = item->data(0, Qt::UserRole);
    if (!var.canConvert<QPair<QString,int>>())
    {
        QMessageBox::warning(this, "Ошибка", "Выберите объект (не передатчик)");
        return;
    }

    auto pair = var.value<QPair<QString,int>>();
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
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Выберите элемент для удаления");
        return;
    }

    QTreeWidgetItem* item = selected.first();
    QVariant var = item->data(0, Qt::UserRole);
    if (!var.canConvert<QPair<QString,int>>())
        return;

    auto pair = var.value<QPair<QString,int>>();
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
        table->setRowCount(0);
        currentTransmitter = -1;
    }
    else
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось удалить элемент: " + query.lastError().text());
    }
}

void MainWindow::loadSpecs(int transmitterID)
{
    table->blockSignals(true);
    table->setRowCount(0);

    QSqlQuery query;
    query.prepare("SELECT power_watt, gain_db, antenna_height FROM specs WHERE transmitter_id = :id");
    query.bindValue(":id", transmitterID);

    if (query.exec() && query.next())
    {
        double power = query.value(0).toDouble();
        double gain = query.value(1).toDouble();
        double height = query.value(2).toDouble();

        QStringList params = { "Мощность (Вт)", "КУ (дБ)", "Высота подвеса (м)" };
        QVector<double> values = { power, gain, height };

        table->setRowCount(params.size());
        for (int row = 0; row < params.size(); ++row)
        {
            QTableWidgetItem* paramItem = new QTableWidgetItem(params[row]);
            paramItem->setFlags(paramItem->flags() & ~Qt::ItemIsEditable);
            table->setItem(row, 0, paramItem);

            QTableWidgetItem* valueItem = new QTableWidgetItem(QString::number(values[row]));
            table->setItem(row, 1, valueItem);
        }
    }
    else
    {
        table->setRowCount(0);
    }

    table->blockSignals(false);
}