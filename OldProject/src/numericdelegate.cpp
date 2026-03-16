#include "numericdelegate.h"
#include <QDoubleSpinBox>
#include <QModelIndex>

NumericDelegate::NumericDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *NumericDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    if (index.column() == 1) {  // Column "Value"
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setMinimum(-999999);
        editor->setMaximum(999999);
        editor->setDecimals(2);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void NumericDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>(editor);
    if (spinBox) {
        bool ok;
        double value = index.model()->data(index, Qt::EditRole).toDouble(&ok);
        if (ok) {
            spinBox->setValue(value);
        } else {
            spinBox->setValue(0.0);
        }
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void NumericDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>(editor);
    if (spinBox) {
        model->setData(index, spinBox->value(), Qt::EditRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
