#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QTableView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QPair>                // добавлено для использования QPair
#include <QMetaType>             // для Q_DECLARE_METATYPE

// Объявляем метатип для QPair<QString,int>, чтобы он корректно работал с QVariant
Q_DECLARE_METATYPE(QPair<QString, int>)

// Делегат для дерева (запрещает редактирование элементов с пользовательскими данными)
class TreeModelDelegate : public QItemDelegate {
	Q_OBJECT
public:
	explicit TreeModelDelegate(QObject* parent = nullptr)
		: QItemDelegate(parent)
	{
	}

	QWidget* createEditor(QWidget* parent,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{
		// Если в элементе есть пользовательские данные (object/transmitter) — запрещаем редактирование
		if (index.data(Qt::UserRole).canConvert<QPair<QString, int>>()) {
			return nullptr;
		}
		// Иначе используем стандартный редактор
		return QItemDelegate::createEditor(parent, option, index);
	}
};

// Модель для отображения ТТХ передатчика в виде таблицы "Параметр - Значение"
class SpecTableModel : public QAbstractTableModel {
	Q_OBJECT
public:
	explicit SpecTableModel(QObject* parent = nullptr);

	void setTransmitterId(int id);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
	struct SpecItem {
		QString name;
		double value;
		QString fieldName;
	};
	QVector<SpecItem> m_specs;
	int m_transmitterId;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

private slots:
	void onTreeClicked(const QModelIndex& index);
	void addObject();
	void addTransmitter();
	void deleteElement();

private:
	void createUI();
	void loadTree();               // загружает данные в дерево из БД (теперь с иконками)
	void loadSpecs(int transmitterID);

	QTreeView* treeView;
	QTableView* tableView;
	QStandardItemModel* treeModel;
	SpecTableModel* specModel;
	QPushButton* addObjectBtn;
	QPushButton* addTransmitterBtn;
	QPushButton* deleteBtn;

	int currentTransmitter;
};

#endif // MAINWINDOW_H