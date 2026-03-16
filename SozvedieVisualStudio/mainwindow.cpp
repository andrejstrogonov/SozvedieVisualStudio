#include "mainwindow.h"
#include "numericdelegate.h"

#include <QtSql>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>

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

    QSqlQuery query("SELECT id,name,latitude,longitude FROM objects");

    while (query.next())
    {
        int objID = query.value(0).toInt();
        QString name = query.value(1).toString();

        QTreeWidgetItem* obj = new QTreeWidgetItem(tree);
        obj->setText(0, name);
        obj->setData(0, Qt::UserRole, objID);

        QSqlQuery tx;
        tx.prepare("SELECT id,name FROM transmitters WHERE object_id=:id");
        tx.bindValue(":id", objID);
        tx.exec();

        while (tx.next())
        {
            int txID = tx.value(0).toInt();

            QTreeWidgetItem* t = new QTreeWidgetItem(obj);
            t->setText(0, tx.value(1).toString());
            t->setData(0, Qt::UserRole + 1, txID);
        }
    }
}