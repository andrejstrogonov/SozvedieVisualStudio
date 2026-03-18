#ifndef NUMERICDELEGATE_H
#define NUMERICDELEGATE_H

#include <QStyledItemDelegate>

class NumericDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit NumericDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor,
        const QModelIndex& index) const override;

    void setModelData(QWidget* editor,
        QAbstractItemModel* model,
        const QModelIndex& index) const override;
};

#endif // NUMERICDELEGATE_H
