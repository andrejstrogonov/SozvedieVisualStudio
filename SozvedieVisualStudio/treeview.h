#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>

class TreeView : public QObject {
    Q_OBJECT

public:
    explicit TreeView(QObject* parent = nullptr);

    Q_INVOKABLE void loadTree();
    Q_INVOKABLE void addObject();
    Q_INVOKABLE void addTransmitter(int objectId);
    Q_INVOKABLE void deleteObject(int id);
    Q_INVOKABLE void deleteTransmitter(int id);

    Q_PROPERTY(QObject* treeModel READ treeModel)
        Q_PROPERTY(QObject* specModel READ specModel)

private:
    QObject* m_treeModel;
    QObject* m_specModel;
};

#endif // TREEVIEW_H
