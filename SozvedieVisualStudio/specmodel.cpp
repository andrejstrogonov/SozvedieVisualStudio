#include "specmodel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

SpecModel::SpecModel(QObject* parent)
    : QAbstractTableModel(parent), m_transmitterId(-1) {
}

void SpecModel::setTransmitterId(int id) {
    beginResetModel();
    m_transmitterId = id;
    m_specs.clear();

    if (id != -1) {
        QSqlQuery query;
        query.prepare("SELECT power_watt, gain_db, antenna_height FROM specs WHERE transmitter_id = :id");
        query.bindValue(":id", id);
        if (query.exec() && query.next()) {
            double power = query.value(0).toDouble();
            double gain = query.value(1).toDouble();
            double height = query.value(2).toDouble();

            m_specs.append({ "Мощность (Вт)", power, "power_watt" });
            m_specs.append({ "КУ (дБ)", gain, "gain_db" });
            m_specs.append({ "Высота подвеса (м)", height, "antenna_height" });
        }
    }
    endResetModel();
}

int SpecModel::rowCount(const QModelIndex& parent) const {
    return m_specs.size();
}

int SpecModel::columnCount(const QModelIndex& parent) const {
    return 2;
}

QVariant SpecModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_specs.size() || index.column() >= 2)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == 0)
            return m_specs[index.row()].name;
        else
            return m_specs[index.row()].value;
    }
    return QVariant();
}

QVariant SpecModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) return "Параметр";
        if (section == 1) return "Значение";
    }
    return QVariant();
}

bool SpecModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role != Qt::EditRole || !index.isValid() || index.row() >= m_specs.size() || index.column() != 1)
        return false;

    double newValue = value.toDouble();
    SpecItem& item = m_specs[index.row()];
    if (qFuzzyCompare(item.value, newValue))
        return false;

    QSqlQuery query;
    query.prepare(QString("UPDATE specs SET %1 = :value WHERE transmitter_id = :id").arg(item.fieldName));
    query.bindValue(":value", newValue);
    query.bindValue(":id", m_transmitterId);

    if (query.exec()) {
        item.value = newValue;
        emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });
        return true;
    }
    return false;
}

Qt::ItemFlags SpecModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.column() == 1)
        flags |= Qt::ItemIsEditable;
    return flags;
}
