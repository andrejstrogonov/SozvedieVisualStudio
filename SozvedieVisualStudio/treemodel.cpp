#include "treemodel.h"
#include <QDebug>

TreeModel::TreeModel(QObject* parent)
    : QObject(parent) {
}

QQmlListProperty<QObject> TreeModel::objects() {
    return QQmlListProperty<QObject>(this, &m_objects);
}

void TreeModel::loadTree() {
    // Имитация загрузки дерева из БД
    m_objects.clear();
    for (int i = 0; i < 3; ++i) {
        QObject* obj = new QObject(this);
        obj->setProperty("name", "Объект " + QString::number(i));
        obj->setProperty("id", i + 1);
        obj->setProperty("type", "object");
        m_objects.append(obj);
    }
}

void TreeModel::addObject() {
    // Имитация добавления объекта
    QObject* obj = new QObject(this);
    obj->setProperty("name", "Новый объект");
    obj->setProperty("id", m_objects.size() + 1);
    obj->setProperty("type", "object");
    m_objects.append(obj);
}

void TreeModel::addTransmitter(int objectId) {
    // Имитация добавления передатчика
    QObject* transmitter = new QObject(this);
    transmitter->setProperty("name", "Передатчик");
    transmitter->setProperty("id", m_objects.size() + 1);
    transmitter->setProperty("type", "transmitter");
    m_objects.append(transmitter);
}

void TreeModel::deleteObject(int id) {
    // Имитация удаления объекта
    for (int i = 0; i < m_objects.size(); ++i) {
        if (m_objects[i]->property("id").toInt() == id) {
            m_objects.removeAt(i);
            break;
        }
    }
}

void TreeModel::deleteTransmitter(int id) {
    // Имитация удаления передатчика
    for (int i = 0; i < m_objects.size(); ++i) {
        if (m_objects[i]->property("id").toInt() == id) {
            m
        }
    }
}
