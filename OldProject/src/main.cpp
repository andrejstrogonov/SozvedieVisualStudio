#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "postgresapp.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Create the PostgreSQL app backend
    PostgresApp postgresApp;

    // Register the backend with QML
    engine.rootContext()->setContextProperty("postgresApp", &postgresApp);

    // Load the QML file
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
