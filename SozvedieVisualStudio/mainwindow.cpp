#include "mainwindow.h"
#include "numericdelegate.h"
#include <QtSql>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QPair>
#include <QStandardItem>
#include <QItemSelectionModel>
#include <QIcon>
#include <QFileDialog>
#include <QStandardPaths>

// ====================== MainWindow ======================
MainWindow::MainWindow() {
	currentTransmitter = -1;

	createUI();
	loadTree();
}

void MainWindow::createUI() {
	QWidget* central = new QWidget;
	setCentralWidget(central);

	QHBoxLayout* mainLayout = new QHBoxLayout;

	// Левая панель: дерево
	treeView = new QTreeView;
	treeModel = new QStandardItemModel;
	treeView->setModel(treeModel);
	treeView->setItemDelegate(new TreeModelDelegate(this));
	treeView->setHeaderHidden(true);

	addObjectBtn = new QPushButton("Add Object");
	addTransmitterBtn = new QPushButton("Add Transmitter");
	deleteBtn = new QPushButton("Delete");

	connect(treeView, &QTreeView::clicked, this, &MainWindow::onTreeClicked);
	connect(addObjectBtn, &QPushButton::clicked, this, &MainWindow::addObject);
	connect(addTransmitterBtn, &QPushButton::clicked, this, &MainWindow::addTransmitter);
	connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteElement);

	QVBoxLayout* leftLayout = new QVBoxLayout;
	leftLayout->addWidget(treeView);
	leftLayout->addWidget(addObjectBtn);
	leftLayout->addWidget(addTransmitterBtn);
	leftLayout->addWidget(deleteBtn); // исправлено

	// Правая панель: таблица ТТХ
	tableView = new QTableView;
	specModel = new SpecTableModel(this);
	tableView->setModel(specModel);
	tableView->setItemDelegate(new NumericDelegate(this));

	tableView->horizontalHeader()->setStretchLastSection(true);
	tableView->verticalHeader()->hide();
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

	QVBoxLayout* rightLayout = new QVBoxLayout;
	rightLayout->addWidget(tableView);

	mainLayout->addLayout(leftLayout, 1);
	mainLayout->addLayout(rightLayout, 2);
	central->setLayout(mainLayout);

	resize(1200, 700);
}

void MainWindow::loadTree() {
	treeModel->clear();
	treeModel->setColumnCount(1);

	QSqlQuery query("SELECT id, name, latitude, longitude, icon_path FROM objects");
	while (query.next()) {
		int objID = query.value(0).toInt();
		QString name = query.value(1).toString();
		double lat = query.value(2).toDouble();
		double lon = query.value(3).toDouble();
		QString iconPath = query.value(4).toString();

		QString displayText = QString("%1 (%2, %3)").arg(name).arg(lat).arg(lon);
		QStandardItem* objItem = new QStandardItem(displayText);
		if (!iconPath.isEmpty()) {
			objItem->setIcon(QIcon(iconPath));
		}
		objItem->setData(QVariant::fromValue(qMakePair(QString("object"), objID)), Qt::UserRole);
		objItem->setEditable(false);

		QSqlQuery txQuery;
		txQuery.prepare("SELECT id, name, icon_path FROM transmitters WHERE object_id = :id");
		txQuery.bindValue(":id", objID);
		txQuery.exec();

		while (txQuery.next()) {
			int txID = txQuery.value(0).toInt();
			QString txName = txQuery.value(1).toString();
			QString txIconPath = txQuery.value(2).toString();

			QStandardItem* txItem = new QStandardItem(txName);
			if (!txIconPath.isEmpty()) {
				txItem->setIcon(QIcon(txIconPath));
			}
			txItem->setData(QVariant::fromValue(qMakePair(QString("transmitter"), txID)), Qt::UserRole);
			txItem->setEditable(false);

			objItem->appendRow(txItem);
		}

		treeModel->appendRow(objItem);
	}

	treeView->expandAll();
}

void MainWindow::onTreeClicked(const QModelIndex& index) {
	if (!index.isValid()) {
		specModel->setTransmitterId(-1);
		currentTransmitter = -1;
		return;
	}

	QVariant var = index.data(Qt::UserRole);
	if (!var.canConvert<QPair<QString, int>>()) {
		specModel->setTransmitterId(-1);
		currentTransmitter = -1;
		return;
	}

	auto pair = var.value<QPair<QString, int>>();
	QString type = pair.first;
	int id = pair.second;

	if (type == "transmitter") {
		currentTransmitter = id;
		specModel->setTransmitterId(id);
	}
	else {
		specModel->setTransmitterId(-1);
		currentTransmitter = -1;
	}
}

void MainWindow::addObject() {
	bool ok;
	QString name = QInputDialog::getText(this, "Добавить объект", "Введите имя объекта:", QLineEdit::Normal, "", &ok);
	if (!ok || name.isEmpty())
		return;

	double lat = QInputDialog::getDouble(this, "Координаты", "Широта:", 0, -90, 90, 4, &ok);
	if (!ok)
		return;
	double lon = QInputDialog::getDouble(this, "Координаты", "Долгота:", 0, -180, 180, 4, &ok);
	if (!ok)
		return;

	QString iconPath = QFileDialog::getOpenFileName(this,
		tr("Выберите иконку для объекта"),
		QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
		tr("Images (*.png *.jpg *.jpeg *.bmp *.svg)"));

	QSqlQuery query;
	query.prepare("INSERT INTO objects (name, latitude, longitude, icon_path) VALUES (:name, :lat, :lon, :icon)");
	query.bindValue(":name", name);
	query.bindValue(":lat", lat);
	query.bindValue(":lon", lon);
	query.bindValue(":icon", iconPath.isEmpty() ? QVariant(QVariant::String) : iconPath);

	if (query.exec()) {
		loadTree();
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Не удалось добавить объект: " + query.lastError().text());
	}
}

void MainWindow::addTransmitter() {
	QList<QModelIndex> selected = treeView->selectionModel()->selectedIndexes();
	if (selected.isEmpty()) {
		QMessageBox::warning(this, "Ошибка", "Выберите объект для добавления передатчика");
		return;
	}

	QModelIndex index = selected.first();
	QVariant var = index.data(Qt::UserRole);
	if (!var.canConvert<QPair<QString, int>>()) {
		QMessageBox::warning(this, "Ошибка", "Выберите объект (не передатчик)");
		return;
	}

	auto pair = var.value<QPair<QString, int>>();
	if (pair.first != "object") {
		QMessageBox::warning(this, "Ошибка", "Выберите объект (не передатчик)");
		return;
	}

	int objectId = pair.second;

	bool ok;
	QString name = QInputDialog::getText(this, "Добавить передатчик", "Введите имя передатчика:", QLineEdit::Normal, "", &ok);
	if (!ok || name.isEmpty())
		return;

	QString iconPath = QFileDialog::getOpenFileName(this,
		tr("Выберите иконку для передатчика"),
		QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
		tr("Images (*.png *.jpg *.jpeg *.bmp *.svg)"));

	QSqlDatabase::database().transaction();

	QSqlQuery query;
	query.prepare("INSERT INTO transmitters (object_id, name, icon_path) VALUES (:obj, :name, :icon)");
	query.bindValue(":obj", objectId);
	query.bindValue(":name", name);
	query.bindValue(":icon", iconPath.isEmpty() ? QVariant(QVariant::String) : iconPath);

	if (query.exec()) {
		int txId = query.lastInsertId().toInt();

		QSqlQuery specQuery;
		specQuery.prepare("INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (:id, 0.0, 0.0, 0.0)");
		specQuery.bindValue(":id", txId);

		if (specQuery.exec()) {
			QSqlDatabase::database().commit();
			loadTree();
		}
		else {
			QSqlDatabase::database().rollback();
			QMessageBox::critical(this, "Ошибка", "Не удалось создать ТТХ для передатчика: " + specQuery.lastError().text());
		}
	}
	else {
		QSqlDatabase::database().rollback();
		QMessageBox::critical(this, "Ошибка", "Не удалось добавить передатчик: " + query.lastError().text());
	}
}

void MainWindow::deleteElement() {
	QList<QModelIndex> selected = treeView->selectionModel()->selectedIndexes();
	if (selected.isEmpty()) {
		QMessageBox::warning(this, "Ошибка", "Выберите элемент для удаления");
		return;
	}

	QModelIndex index = selected.first();
	QVariant var = index.data(Qt::UserRole);
	if (!var.canConvert<QPair<QString, int>>())
		return;

	auto pair = var.value<QPair<QString, int>>();
	QString type = pair.first;
	int id = pair.second;

	QString message;
	if (type == "object")
		message = "Вы уверены, что хотите удалить объект и все его передатчики?";
	else if (type == "transmitter")
		message = "Вы уверены, что хотите удалить этот передатчик?";
	else
		return;

	int reply = QMessageBox::question(this, "Подтверждение", message, QMessageBox::Yes | QMessageBox::No);
	if (reply != QMessageBox::Yes)
		return;

	QSqlQuery query;
	if (type == "object")
		query.prepare("DELETE FROM objects WHERE id = :id");
	else
		query.prepare("DELETE FROM transmitters WHERE id = :id");

	query.bindValue(":id", id);

	if (query.exec()) {
		loadTree();
		specModel->setTransmitterId(-1);
		currentTransmitter = -1;
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Не удалось удалить элемент: " + query.lastError().text());
	}
}

void MainWindow::loadSpecs(int transmitterID) {
	specModel->setTransmitterId(transmitterID);
}

// ====================== SpecTableModel Implementation ======================
SpecTableModel::SpecTableModel(QObject* parent)
	: QAbstractTableModel(parent), m_transmitterId(-1) {
}

void SpecTableModel::setTransmitterId(int id) {
	beginResetModel();
	m_transmitterId = id;
	m_specs.clear();

	if (id != -1) {
		QSqlQuery query;
		query.prepare("SELECT power_watt, gain_db, antenna_height FROM specs WHERE transmitter_id = :id");
		query.bindValue(":id", id);
		if (query.exec() && query.next()) {
			double power = query.value(0).toDouble();
			double gain = query.value(1).toDouble();
			double height = query.value(2).toDouble();

			m_specs.append({ "Мощность (Вт)", power, "power_watt" });
			m_specs.append({ "КУ (дБ)", gain, "gain_db" });
			m_specs.append({ "Высота подвеса (м)", height, "antenna_height" });
		}
	}
	endResetModel();
}

int SpecTableModel::rowCount(const QModelIndex&) const {
	return m_specs.size();
}

int SpecTableModel::columnCount(const QModelIndex&) const {
	return 2;
}

QVariant SpecTableModel::data(const QModelIndex& index, int role) const {
	if (!index.isValid() || index.row() >= m_specs.size() || index.column() >= 2)
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		if (index.column() == 0)
			return m_specs[index.row()].name;
		else
			return m_specs[index.row()].value;
	}
	return QVariant();
}

QVariant SpecTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		if (section == 0) return "Параметр";
		if (section == 1) return "Значение";
	}
	return QVariant();
}

bool SpecTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	if (role != Qt::EditRole || !index.isValid() || index.row() >= m_specs.size() || index.column() != 1)
		return false;

	double newValue = value.toDouble();
	SpecItem& item = m_specs[index.row()];
	if (qFuzzyCompare(item.value, newValue))
		return false;

	QSqlQuery query;
	query.prepare(QString("UPDATE specs SET %1 = :value WHERE transmitter_id = :id").arg(item.fieldName));
	query.bindValue(":value", newValue);
	query.bindValue(":id", m_transmitterId);

	if (query.exec()) {
		item.value = newValue;
		emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });
		return true;
	}
	return false;
}

Qt::ItemFlags SpecTableModel::flags(const QModelIndex& index) const {
	Qt::ItemFlags flags = QAbstractTableModel::flags(index);
	if (index.column() == 1)
		flags |= Qt::ItemIsEditable;
	return flags;
}