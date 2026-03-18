#include "treeview.h"

TreeView::TreeView(QObject* parent)
    : QObject(parent), m_treeModel(new QObject(this)), m_specModel(new SpecModel(this)) {
}

QObject* TreeView::treeModel() {
    return m_treeModel;
}

QObject* TreeView::specModel() {
    return m_specModel;
}

void TreeView::loadTree() {
    // Имитация загрузки дерева из БД
}

void TreeView::addObject() {
    // Имитация добавления объекта
}

void TreeView::addTransmitter(int objectId) {
    // Имитация добавления передатчика
}

void TreeView::deleteObject(int id) {
    // Имитация удаления объекта
}

void TreeView::deleteTransmitter(int id) {
    // Имитация удаления передатчика
}
