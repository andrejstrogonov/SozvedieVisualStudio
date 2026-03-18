// database.cpp
#include "database.h"
#include <QMessageBox>
#include <QDebug>

bool Database::connect()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
	db.setHostName("localhost");
	db.setDatabaseName("mydb_astra");
	db.setUserName("postgres");
	db.setPassword("postgres");
	db.setPort(5432);

	if (!db.open()) {
		qCritical() << "DB connection error:" << db.lastError().text();
		return false;
	}

	QSqlQuery query;
	query.exec("CREATE TABLE IF NOT EXISTS objects ("
		"id SERIAL PRIMARY KEY, "
		"name TEXT, "
		"latitude REAL, "
		"longitude REAL, "
		"icon_path TEXT)");
	query.exec("CREATE TABLE IF NOT EXISTS transmitters ("
		"id SERIAL PRIMARY KEY, "
		"object_id INTEGER REFERENCES objects(id) ON DELETE CASCADE, "
		"name TEXT, "
		"icon_path TEXT)");
	query.exec("CREATE TABLE IF NOT EXISTS specs ("
		"id SERIAL PRIMARY KEY, "
		"transmitter_id INTEGER REFERENCES transmitters(id) ON DELETE CASCADE, "
		"power_watt REAL, "
		"gain_db REAL, "
		"antenna_height REAL)");
	return true;
}