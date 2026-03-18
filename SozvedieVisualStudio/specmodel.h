// specmodel.h (аналогично приведённому в задании, но с небольшими исправлениями)
#ifndef SPECMODEL_H
#define SPECMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QSqlQuery>
#include <QSqlError>

class SpecModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit SpecModel(QObject* parent = nullptr);

	void setTransmitterId(int id);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
	struct SpecItem {
		QString name;       // отображаемое имя
		double value;       // текущее значение
		QString fieldName;  // имя поля в БД
	};
	QVector<SpecItem> m_specs;
	int m_transmitterId;
};

#endif // SPECMODEL_H