// specmodel.cpp
#include "specmodel.h"
#include <QSqlRecord>
#include <QDebug>

SpecModel::SpecModel(QObject* parent)
	: QAbstractTableModel(parent)
	, m_transmitterId(-1)
{
}

void SpecModel::setTransmitterId(int id)
{
	beginResetModel();
	m_specs.clear();
	m_transmitterId = id;

	if (id != -1) {
		QSqlQuery q;
		q.prepare("SELECT power_watt, gain_db, antenna_height FROM specs WHERE transmitter_id = ?");
		q.addBindValue(id);
		if (q.exec() && q.next()) {
			double power = q.value(0).toDouble();
			double gain = q.value(1).toDouble();
			double height = q.value(2).toDouble();

			m_specs.append({ "Мощность (Вт)", power, "power_watt" });
			m_specs.append({ "КУ (дБ)", gain, "gain_db" });
			m_specs.append({ "Высота подвеса (м)", height, "antenna_height" });
		}
	}
	endResetModel();
}

int SpecModel::rowCount(const QModelIndex&) const
{
	return m_specs.size();
}

int SpecModel::columnCount(const QModelIndex&) const
{
	return 2; // параметр | значение
}

QVariant SpecModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() >= m_specs.size())
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		if (index.column() == 0)
			return m_specs[index.row()].name;
		else
			return m_specs[index.row()].value;
	}
	return QVariant();
}

QVariant SpecModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		if (section == 0) return "Параметр";
		if (section == 1) return "Значение";
	}
	return QVariant();
}

bool SpecModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role != Qt::EditRole || !index.isValid() || index.column() != 1 || index.row() >= m_specs.size())
		return false;

	double newVal = value.toDouble();
	SpecItem& item = m_specs[index.row()];
	if (qFuzzyCompare(item.value, newVal))
		return false;

	QSqlQuery q;
	q.prepare(QString("UPDATE specs SET %1 = ? WHERE transmitter_id = ?").arg(item.fieldName));
	q.addBindValue(newVal);
	q.addBindValue(m_transmitterId);
	if (q.exec()) {
		item.value = newVal;
		emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });
		return true;
	}
	return false;
}

Qt::ItemFlags SpecModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags f = QAbstractTableModel::flags(index);
	if (index.column() == 1)
		f |= Qt::ItemIsEditable;
	return f;
}