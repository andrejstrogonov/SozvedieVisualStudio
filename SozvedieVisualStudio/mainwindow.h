#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QTableView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QAbstractTableModel>
#include <QItemDelegate>          // обязательно для базового класса
#include <QStyledItemDelegate>    // (можно использовать и его, но для совместимости оставим QItemDelegate)

// Делегат для дерева (запрещает редактирование элементов с пользовательскими данными)
class TreeModelDelegate : public QItemDelegate {
	Q_OBJECT   // ← добавляем макрос, чтобы MOC сгенерировал метаинформацию
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
		// Иначе используем стандартный редактор (например, для обычных текстовых элементов)
		return QItemDelegate::createEditor(parent, option, index);
	}
};

// Модель для отображения ТТХ передатчика в виде таблицы "Параметр - Значение"
class SpecTableModel : public QAbstractTableModel {
	Q_OBJECT
public:
	explicit SpecTableModel(QObject* parent = nullptr);

	// Задать ID передатчика и загрузить его характеристики из БД
	void setTransmitterId(int id);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
	struct SpecItem {
		QString name;       // Отображаемое имя параметра
		double value;       // Текущее значение
		QString fieldName;  // Имя поля в таблице БД (для обновления)
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
	void onTreeClicked(const QModelIndex& index);   // обработчик клика по дереву
	void addObject();
	void addTransmitter();
	void deleteElement();

private:
	void createUI();
	void loadTree();               // загружает данные в дерево из БД
	void loadSpecs(int transmitterID); // загружает ТТХ конкретного передатчика

	QTreeView* treeView;
	QTableView* tableView;
	QStandardItemModel* treeModel;
	SpecTableModel* specModel;      // кастомная модель для таблицы ТТХ
	QPushButton* addObjectBtn;
	QPushButton* addTransmitterBtn;
	QPushButton* deleteBtn;

	int currentTransmitter;          // ID текущего выбранного передатчика (или -1)
};

#endif // MAINWINDOW_H