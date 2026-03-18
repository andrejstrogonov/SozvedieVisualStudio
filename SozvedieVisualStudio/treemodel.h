#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QQmlListProperty>
#include <QObject>

class TreeModel : public QObject {
    Q_OBJECT

public:
    explicit TreeModel(QObject* parent = nullptr);

    QQmlListProperty<QObject> objects();

    void addObject();
    void addTransmitter(int objectId);
    void deleteObject(int id);
    void deleteTransmitter(int id);

    Q_INVOKABLE void loadTree();

private:
    QList<QObject*> m_objects;
};
#endif // TREEMODEL_H
