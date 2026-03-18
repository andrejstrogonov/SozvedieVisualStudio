// objecttreemodel.h
#ifndef OBJECTTREEMODEL_H
#define OBJECTTREEMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QPair>
#include <QSqlQuery>
#include <QIcon>

class ObjectTreeModel : public QAbstractItemModel
{
	Q_OBJECT
		Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

public:
	enum Roles {
		TypeRole = Qt::UserRole + 1,
		IdRole,
		NameRole,
		IconSourceRole,
		LatRole,    // только для объектов
		LonRole
	};

	explicit ObjectTreeModel(QObject* parent = nullptr);

	// QAbstractItemModel interface
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& child) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

	bool ready() const { return m_ready; }

public slots:
	void refresh();                          // перезагрузить из БД
	void addObject(const QString& name, double lat, double lon, const QString& iconPath);
	void addTransmitter(int objectId, const QString& name, const QString& iconPath);
	void deleteItem(const QModelIndex& index);

signals:
	void readyChanged();

private:
	struct Node {
		int id;                 // id объекта или передатчика
		QString type;           // "object" или "transmitter"
		QString name;
		QString iconPath;
		double lat;             // для объекта
		double lon;             // для объекта
		QList<Node*> children;  // для объектов – дочерние передатчики
		Node* parent;

		Node() : parent(nullptr), lat(0), lon(0) {}
		~Node() { qDeleteAll(children); }
	};

	Node* m_root;
	bool m_ready;

	void clear();
	void loadData();
	Node* nodeFromIndex(const QModelIndex& index) const;
	QModelIndex indexFromNode(Node* node, int column = 0) const;
};

#endif // OBJECTTREEMODEL_H