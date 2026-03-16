#include "database.h"
#include <QMessageBox>

bool Database::connect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");

    db.setHostName("localhost");
    db.setDatabaseName("mydb");
    db.setUserName("postgres");
    db.setPassword("postgres");
    db.setPort(5432);

    if (!db.open())
    {
        QMessageBox::critical(nullptr, "DB Error", db.lastError().text());
        return false;
    }

    return true;
}