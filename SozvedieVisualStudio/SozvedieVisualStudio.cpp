// SozvedieVisualStudio.cpp: определяет точку входа для приложения.
//

#include "SozvedieVisualStudio.h"
#include <QApplication>
#include "mainwindow.h"
#include "database.h"


int main()
{
    QApplication app(argc, argv);

    if (!Database::connect())
        return -1;

    MainWindow w;
    w.show();

    return app.exec();
}
