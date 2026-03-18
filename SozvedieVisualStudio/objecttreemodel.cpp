// objecttreemodel.cpp
#include "objecttreemodel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QUrl>

ObjectTreeModel::ObjectTreeModel(QObject* parent)
	: QAbstractItemModel(parent)
	, m_root(new Node)
	, m_ready(false)
{
	m_root->type = "root";
	refresh();
}

void ObjectTreeModel::refresh()
{
	beginResetModel();
	clear();
	loadData();
	endResetModel();
	if (!m_ready) {
		m_ready = true;
		emit readyChanged();
	}
}

void ObjectTreeModel::clear()
{
	qDeleteAll(m_root->children);
	m_root->children.clear();
}

void ObjectTreeModel::loadData()
{
	QSqlQuery query("SELECT id, name, latitude, longitude, icon_path FROM objects ORDER BY id");
	while (query.next()) {
		Node* objNode = new Node;
		objNode->id = query.value(0).toInt();
		objNode->type = "object";
		objNode->name = query.value(1).toString();
		objNode->lat = query.value(2).toDouble();
		objNode->lon = query.value(3).toDouble();
		objNode->iconPath = query.value(4).toString();
		objNode->parent = m_root;
		m_root->children.append(objNode);

		QSqlQuery txQuery;
		txQuery.prepare("SELECT id, name, icon_path FROM transmitters WHERE object_id = :oid ORDER BY id");
		txQuery.bindValue(":oid", objNode->id);
		txQuery.exec();
		while (txQuery.next()) {
			Node* txNode = new Node;
			txNode->id = txQuery.value(0).toInt();
			txNode->type = "transmitter";
			txNode->name = txQuery.value(1).toString();
			txNode->iconPath = txQuery.value(2).toString();
			txNode->parent = objNode;
			objNode->children.append(txNode);
		}
	}
}

QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	Node* parentNode = (parent.isValid()) ? static_cast<Node*>(parent.internalPointer()) : m_root;
	if (row >= parentNode->children.size())
		return QModelIndex();

	Node* childNode = parentNode->children.at(row);
	return createIndex(row, column, childNode);
}

QModelIndex ObjectTreeModel::parent(const QModelIndex& child) const
{
	if (!child.isValid())
		return QModelIndex();

	Node* childNode = static_cast<Node*>(child.internalPointer());
	Node* parentNode = childNode->parent;
	if (parentNode == m_root)
		return QModelIndex();

	return indexFromNode(parentNode);
}

int ObjectTreeModel::rowCount(const QModelIndex& parent) const
{
	if (parent.column() > 0)
		return 0;

	Node* parentNode = (parent.isValid()) ? static_cast<Node*>(parent.internalPointer()) : m_root;
	return parentNode->children.size();
}

int ObjectTreeModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 1; // один столбец (имя)
}

QVariant ObjectTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	Node* node = static_cast<Node*>(index.internalPointer());
	if (!node)
		return QVariant();

	switch (role) {
	case Qt::DisplayRole:
	case NameRole:
		return node->name;
	case TypeRole:
		return node->type;
	case IdRole:
		return node->id;
	case IconSourceRole:
		return node->iconPath.isEmpty() ? QVariant() : QUrl::fromLocalFile(node->iconPath);
	case LatRole:
		return (node->type == "object") ? node->lat : QVariant();
	case LonRole:
		return (node->type == "object") ? node->lon : QVariant();
	default:
		return QVariant();
	}
}

QHash<int, QByteArray> ObjectTreeModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[TypeRole] = "type";
	roles[IdRole] = "id";
	roles[NameRole] = "name";
	roles[IconSourceRole] = "iconSource";
	roles[LatRole] = "latitude";
	roles[LonRole] = "longitude";
	return roles;
}

void ObjectTreeModel::addObject(const QString& name, double lat, double lon, const QString& iconPath)
{
	QSqlQuery q;
	q.prepare("INSERT INTO objects (name, latitude, longitude, icon_path) VALUES (?,?,?,?)");
	q.addBindValue(name);
	q.addBindValue(lat);
	q.addBindValue(lon);
	q.addBindValue(iconPath.isEmpty() ? QVariant() : iconPath);
	if (!q.exec()) {
		qCritical() << "addObject error:" << q.lastError().text();
		return;
	}
	refresh();
}

void ObjectTreeModel::addTransmitter(int objectId, const QString& name, const QString& iconPath)
{
	QSqlDatabase::database().transaction();

	QSqlQuery q;
	q.prepare("INSERT INTO transmitters (object_id, name, icon_path) VALUES (?,?,?) RETURNING id");
	q.addBindValue(objectId);
	q.addBindValue(name);
	q.addBindValue(iconPath.isEmpty() ? QVariant() : iconPath);
	if (!q.exec()) {
		QSqlDatabase::database().rollback();
		qCritical() << "addTransmitter error:" << q.lastError().text();
		return;
	}
	int txId = q.next() ? q.value(0).toInt() : -1;

	QSqlQuery specQ;
	specQ.prepare("INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (?,0,0,0)");
	specQ.addBindValue(txId);
	if (!specQ.exec()) {
		QSqlDatabase::database().rollback();
		qCritical() << "add specs error:" << specQ.lastError().text();
		return;
	}

	QSqlDatabase::database().commit();
	refresh();
}

void ObjectTreeModel::deleteItem(const QModelIndex& index)
{
	if (!index.isValid())
		return;

	Node* node = static_cast<Node*>(index.internalPointer());
	if (!node || node->type == "root")
		return;

	QString table = (node->type == "object") ? "objects" : "transmitters";
	QSqlQuery q;
	q.prepare(QString("DELETE FROM %1 WHERE id = ?").arg(table));
	q.addBindValue(node->id);
	if (!q.exec()) {
		qCritical() << "delete error:" << q.lastError().text();
		return;
	}
	refresh();
}

QModelIndex ObjectTreeModel::indexFromNode(Node* node, int column) const
{
	if (!node || node->parent == nullptr)
		return QModelIndex();

	Node* parentNode = node->parent;
	int row = parentNode->children.indexOf(node);
	return createIndex(row, column, node);
}