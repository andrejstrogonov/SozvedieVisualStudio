import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    width: 1200
    height: 700
    title: "Менеджер объектов и ТТХ"

    // Блокировка ресайза
    resizeMode: Window.NoResize

    // Панель навигации (Дерево)
    ColumnLayout {
        id: treePanel
        width: parent.width
        height: parent.height

        // Заголовок
        Text {
            text: "Дерево объектов"
            font.bold: true
            font.pointSize: 16
            Layout.preferredHeight: 30
        }

        // Дерево
        TreeView {
            id: treeView
            width: parent.width
            height: parent.height - 100

            model: treeModel
            clip: true

            // Настройка колонок
            columnWidths: ["200", "100"]
            header: TreeViewHeader {
                visible: false
            }

            // Делегат для узлов
            delegate: ItemDelegate {
                width: parent.width
                text: "Имя"
                contentItem: Text {
                    text: item.name
                    font.bold: true
                }
                background: Rectangle {
                    color: "lightblue"
                }
            }

            // Событие выбора элемента
            onCurrentItemChanged: {
                if (selectedItem) {
                    if (selectedItem.type === "object") {
                        loadSpecs(selectedItem.id)
                    } else if (selectedItem.type === "transmitter") {
                        loadSpecs(selectedItem.id)
                    }
                }
            }
        }

        // Кнопки управления
        RowLayout {
            spacing: 10

            Button {
                text: "Добавить объект"
                onClicked: addObject()
            }

            Button {
                text: "Добавить передатчик"
                onClicked: addTransmitter()
            }

            Button {
                text: "Удалить"
                onClicked: deleteElement()
            }
        }
    }

    // Панель редактирования (Таблица ТТХ)
    ColumnLayout {
        id: tablePanel
        width: parent.width
        height: parent.height

        // Заголовок
        Text {
            text: "Таблица ТТХ"
            font.bold: true
            font.pointSize: 16
            Layout.preferredHeight: 30
        }

        // Таблица
        TableView {
            id: tableView
            width: parent.width
            height: parent.height - 100

            model: specModel
            clip: true

            // Настройка колонок
            columnWidths: ["200", "100"]
            header: TableViewHeader {
                visible: true
            }

            // Делегат для редактирования чисел
            delegate: ItemDelegate {
                width: parent.width
                text: "Значение"
                contentItem: Text {
                    text: model.value
                    font.bold: true
                }
                background: Rectangle {
                    color: "lightgray"
                }

                // Поверхность для редактирования
                contentItem: TextInput {
                    text: model.value
                    onAccepted: {
                        model.value = text
                    }
                }
            }

            // Событие изменения ячейки
            onCurrentItemChanged: {
                if (selectedItem) {
                    updateSpecs(selectedItem.row, selectedItem.column)
                }
            }
        }
    }
}
