#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QTableView>
#include <QSqlTableModel>
#include <QPushButton>
#include <QModelIndex>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:

    QTreeView* treeView;
    QTableView* tableView;
    QSqlTableModel* specModel;
    QAbstractItemModel* treeModel;

    QPushButton* addObjectBtn;
    QPushButton* addTransmitterBtn;
    QPushButton* deleteBtn;

    int currentTransmitter;

    void createUI();
    void loadTree();
    void loadSpecs(int transmitterID);

private slots:

    void onTreeChanged(const QModelIndex& index);  // ✅ Используем QModelIndex
    void onTableChanged(const QModelIndex& index); // ✅ Используем QModelIndex
    void addObject();
    void addTransmitter();
    void deleteElement();
};

#endif // MAINWINDOW_H
