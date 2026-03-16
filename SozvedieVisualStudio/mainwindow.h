#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTableWidget>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:

    QTreeWidget* tree;
    QTableWidget* table;

    QPushButton* addObjectBtn;
    QPushButton* addTransmitterBtn;
    QPushButton* deleteBtn;

    int currentTransmitter;

    void createUI();
    void loadTree();
    void loadSpecs(int transmitterID);

private slots:

    void onTreeChanged();
    void onTableChanged(QTableWidgetItem*);
    void addObject();
    void addTransmitter();
    void deleteElement();
};

#endif