// SozvedieVisualStudio.cpp: определяет точку входа для приложения.
//

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSqlDatabase>
#include "database.h"
#include "objecttreemodel.h"
#include "specmodel.h"

int main(int argc, char* argv[])
{
	QGuiApplication app(argc, argv);

	// Подключение к БД
	if (!Database::connect()) {
		return -1;  // сообщение об ошибке уже выведено в консоль
	}

	// Регистрация типов для QML (опционально)
	qmlRegisterType<ObjectTreeModel>("AppModels", 1, 0, "ObjectTreeModel");
	qmlRegisterType<SpecModel>("AppModels", 1, 0, "SpecModel");

	QQmlApplicationEngine engine;

	// Создаём экземпляры моделей и делаем их доступными в QML через контекст
	ObjectTreeModel treeModel;
	SpecModel specModel;

	engine.rootContext()->setContextProperty("treeModel", &treeModel);
	engine.rootContext()->setContextProperty("specModel", &specModel);

	const QUrl url(QStringLiteral("qrc:/main.qml"));
	engine.load(url);

	return app.exec();
}

