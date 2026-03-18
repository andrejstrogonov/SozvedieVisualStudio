// SozvedieVisualStudio.cpp: определяет точку входа для приложения.
//

#include "SozvedieVisualStudio.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "mainwindow.h"
#include "database.h"


int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (!Database::connect())
        return -1;

    MainWindow w;
    w.show();

    return app.exec();
}
