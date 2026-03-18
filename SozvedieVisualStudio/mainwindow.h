#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QTableView>
#include <QSqlTableModel>
#include <QPushButton>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QPair>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlTableModel>
#include <QHeaderView>
#include <QItemDelegate>

class TreeModelDelegate : public QItemDelegate {
    Q_OBJECT
public:
    TreeModelDelegate(QObject* parent = nullptr) : QItemDelegate(parent) {}

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (index.data(Qt::UserRole).canConvert<QPair<QString, int>>()) {
            return nullptr; // Нет редактора для пользовательских данных
        }
        return QItemDelegate::createEditor(parent, option, index);
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:

    QTreeView* treeView;
    QTableView* tableView;
    QStandardItemModel* treeModel;
    QSqlTableModel* specModel;
    QPushButton* addObjectBtn;
    QPushButton* addTransmitterBtn;
    QPushButton* deleteBtn;

    int currentTransmitter;

    void createUI();
    void loadTree();
    void loadSpecs(int transmitterID);

private slots:

    void onTreeChanged(const QModelIndex& index);
    void onTableChanged(const QModelIndex& index);
    void addObject();
    void addTransmitter();
    void deleteElement();
};

#endif // MAINWINDOW_H
