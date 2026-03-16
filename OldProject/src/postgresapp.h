#ifndef POSTGRESAPP_H
#define POSTGRESAPP_H

#include <QObject>
#include <QSqlDatabase>
#include <QAbstractListModel>
#include <QVariantMap>

class SpecsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SpecsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setSpecs(const QStringList &names, const QList<double> &values);
    void clear();

private:
    QStringList paramNames;
    QList<double> paramValues;
};

class PostgresApp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SpecsModel* specsModel READ specsModel CONSTANT)

public:
    explicit PostgresApp(QObject *parent = nullptr);
    ~PostgresApp();

    SpecsModel* specsModel() const { return m_specsModel; }

public slots:
    void onTreeSelectionChanged(const QVariantMap &model);
    void addObject(const QString &name, double latitude, double longitude);
    void addTransmitter(const QString &name);
    void deleteElement();
    void updateSpec(int index, const QString &value);

private:
    void initDatabase();
    void loadTreeData();
    void loadSpecsTable(int transmitterId);

    QSqlDatabase db;
    SpecsModel *m_specsModel;
    int currentTransmitterId;
    int currentObjectId;
};

#endif // POSTGRESAPP_H
