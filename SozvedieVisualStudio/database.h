// database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class Database {
public:
	static bool connect();
};

#endif // DATABASE_H