#include "numericdelegate.h"
#include <QDoubleSpinBox>

NumericDelegate::NumericDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

QWidget* NumericDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem&,
    const QModelIndex&) const
{
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    editor->setRange(-999999, 999999);
    editor->setDecimals(2);
    return editor;
}

void NumericDelegate::setEditorData(QWidget* editor,
    const QModelIndex& index) const
{
    double value = index.model()->data(index).toDouble();

    QDoubleSpinBox* spin = static_cast<QDoubleSpinBox*>(editor);
    spin->setValue(value);
}

void NumericDelegate::setModelData(QWidget* editor,
    QAbstractItemModel* model,
    const QModelIndex& index) const
{
    QDoubleSpinBox* spin = static_cast<QDoubleSpinBox*>(editor);
    spin->interpretText();

    model->setData(index, spin->value());
}