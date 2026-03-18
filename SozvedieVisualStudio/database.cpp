#include "database.h"
#include <QMessageBox>

bool Database::connect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");

    db.setHostName("localhost");
    db.setDatabaseName("mydb_astra");
    db.setUserName("postgres");
    db.setPassword("postgres");
    db.setPort(5432);

    if (!db.open())
    {
        QMessageBox::critical(nullptr, "DB Error", db.lastError().text());
        return false;
    }
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS objects (id INTEGER PRIMARY KEY, name TEXT, latitude REAL, longitude REAL, icon_path TEXT)");
    query.exec("CREATE TABLE IF NOT EXISTS transmitters (id INTEGER PRIMARY KEY, object_id INTEGER, name TEXT, icon_path TEXT)");
    query.exec("CREATE TABLE IF NOT EXISTS specs (id INTEGER PRIMARY KEY, transmitter_id INTEGER, power_watt REAL, gain_db REAL, antenna_height REAL)");
    return true;
}