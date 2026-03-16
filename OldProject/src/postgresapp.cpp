#include "postgresapp.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// SpecsModel implementation
SpecsModel::SpecsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int SpecsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return paramNames.size();
}

QVariant SpecsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= paramNames.size())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return paramNames[index.row()];
    } else if (role == Qt::UserRole) {
        return paramValues[index.row()];
    } else if (role == Qt::UserRole + 1) {
        return index.row();
    }

    return QVariant();
}

QHash<int, QByteArray> SpecsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "paramName";
    roles[Qt::UserRole] = "paramValue";
    roles[Qt::UserRole + 1] = "index";
    return roles;
}

void SpecsModel::setSpecs(const QStringList &names, const QList<double> &values)
{
    beginResetModel();
    paramNames = names;
    paramValues = values;
    endResetModel();
}

void SpecsModel::clear()
{
    beginResetModel();
    paramNames.clear();
    paramValues.clear();
    endResetModel();
}

// PostgresApp implementation
PostgresApp::PostgresApp(QObject *parent)
    : QObject(parent), currentTransmitterId(-1), currentObjectId(-1)
{
    m_specsModel = new SpecsModel(this);
    initDatabase();
}

PostgresApp::~PostgresApp()
{
    if (db.isOpen()) {
        db.close();
    }
}

void PostgresApp::initDatabase()
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("postgres");
    db.setUserName("postgres");
    db.setPassword("postgres");
    db.setPort(5432);

    if (!db.open()) {
        qCritical() << "Failed to connect to database:" << db.lastError().text();
        return;
    }

    qDebug() << "Connected to PostgreSQL database";
    loadTreeData();
}

void PostgresApp::loadTreeData()
{
    QSqlQuery query(db);
    query.exec("SELECT id, name, latitude, longitude FROM objects ORDER BY name");

    while (query.next()) {
        int objId = query.value(0).toInt();
        QString objName = query.value(1).toString();
        double latitude = query.value(2).toDouble();
        double longitude = query.value(3).toDouble();

        qDebug() << "Object:" << objName << "(" << latitude << "," << longitude << ")";

        // Load transmitters for this object
        QSqlQuery txQuery(db);
        txQuery.exec(QString("SELECT id, name FROM transmitters WHERE object_id = %1 ORDER BY name").arg(objId));

        while (txQuery.next()) {
            int txId = txQuery.value(0).toInt();
            QString txName = txQuery.value(1).toString();

            qDebug() << "  Transmitter:" << txName;

            // Load specs for this transmitter
            QSqlQuery specsQuery(db);
            specsQuery.exec(QString("SELECT id FROM specs WHERE transmitter_id = %1").arg(txId));

            if (specsQuery.next()) {
                qDebug() << "    Specs loaded";
            }
        }
    }
}

void PostgresApp::onTreeSelectionChanged(const QVariantMap &model)
{
    QString itemType = model.value("type", "").toString();
    int itemId = model.value("id", -1).toInt();

    qDebug() << "Tree selection changed:" << itemType << itemId;

    if (itemType == "transmitter") {
        currentTransmitterId = itemId;
        loadSpecsTable(itemId);
    } else if (itemType == "specs") {
        // Get transmitter ID from parent
        currentTransmitterId = model.value("transmitterId", -1).toInt();
        loadSpecsTable(currentTransmitterId);
    } else {
        m_specsModel->clear();
        currentTransmitterId = -1;
    }
}

void PostgresApp::loadSpecsTable(int transmitterId)
{
    QSqlQuery query(db);
    query.exec(QString("SELECT power_watt, gain_db, antenna_height FROM specs WHERE transmitter_id = %1").arg(transmitterId));

    if (query.next()) {
        QStringList paramNames = {"Мощность (Вт)", "КУ (дБ)", "Высота подвеса (м)"};
        QList<double> paramValues = {
            query.value(0).toDouble(),
            query.value(1).toDouble(),
            query.value(2).toDouble()
        };

        m_specsModel->setSpecs(paramNames, paramValues);
    } else {
        m_specsModel->clear();
    }
}

void PostgresApp::addObject(const QString &name, double latitude, double longitude)
{
    if (name.isEmpty()) {
        qWarning() << "Object name cannot be empty";
        return;
    }

    if (latitude < -90.0 || latitude > 90.0) {
        qWarning() << "Invalid latitude";
        return;
    }

    if (longitude < -180.0 || longitude > 180.0) {
        qWarning() << "Invalid longitude";
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO objects (name, latitude, longitude) VALUES (?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(latitude);
    query.addBindValue(longitude);

    if (query.exec()) {
        qDebug() << "Object added successfully";
        loadTreeData();
    } else {
        qCritical() << "Failed to add object:" << query.lastError().text();
    }
}

void PostgresApp::addTransmitter(const QString &name)
{
    if (name.isEmpty()) {
        qWarning() << "Transmitter name cannot be empty";
        return;
    }

    if (currentObjectId == -1) {
        qWarning() << "No object selected";
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO transmitters (object_id, name) VALUES (?, ?)");
    query.addBindValue(currentObjectId);
    query.addBindValue(name);

    if (query.exec()) {
        // Get the ID of the new transmitter
        QSqlQuery lastIdQuery(db);
        lastIdQuery.exec("SELECT lastval()");
        if (lastIdQuery.next()) {
            int txId = lastIdQuery.value(0).toInt();
            // Create default specs
            QSqlQuery specsQuery(db);
            specsQuery.prepare("INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (?, ?, ?, ?)");
            specsQuery.addBindValue(txId);
            specsQuery.addBindValue(0.0);
            specsQuery.addBindValue(0.0);
            specsQuery.addBindValue(0.0);
            specsQuery.exec();
        }

        qDebug() << "Transmitter added successfully";
        loadTreeData();
    } else {
        qCritical() << "Failed to add transmitter:" << query.lastError().text();
    }
}

void PostgresApp::deleteElement()
{
    if (currentTransmitterId == -1 && currentObjectId == -1) {
        qWarning() << "No element selected";
        return;
    }

    QSqlQuery query(db);

    if (currentTransmitterId != -1) {
        query.prepare("DELETE FROM transmitters WHERE id = ?");
        query.addBindValue(currentTransmitterId);
    } else if (currentObjectId != -1) {
        query.prepare("DELETE FROM objects WHERE id = ?");
        query.addBindValue(currentObjectId);
    }

    if (query.exec()) {
        qDebug() << "Element deleted successfully";
        m_specsModel->clear();
        currentTransmitterId = -1;
        currentObjectId = -1;
        loadTreeData();
    } else {
        qCritical() << "Failed to delete element:" << query.lastError().text();
    }
}

void PostgresApp::updateSpec(int index, const QString &value)
{
    if (currentTransmitterId == -1) {
        qWarning() << "No transmitter selected";
        return;
    }

    bool ok;
    double newValue = value.toDouble(&ok);
    if (!ok) {
        qWarning() << "Invalid numeric value";
        return;
    }

    QStringList fieldNames = {"power_watt", "gain_db", "antenna_height"};
    if (index < 0 || index >= fieldNames.size()) {
        qWarning() << "Invalid spec index";
        return;
    }

    QString field = fieldNames[index];
    QSqlQuery query(db);
    query.prepare(QString("UPDATE specs SET %1 = ? WHERE transmitter_id = ?").arg(field));
    query.addBindValue(newValue);
    query.addBindValue(currentTransmitterId);

    if (query.exec()) {
        qDebug() << "Spec updated successfully";
    } else {
        qCritical() << "Failed to update spec:" << query.lastError().text();
    }
}
