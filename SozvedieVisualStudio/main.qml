import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import AppModels 1.0

ApplicationWindow {
    visible: true
    width: 1200
    height: 700
    title: "Менеджер объектов и ТТХ"

    // Модели доступны из контекста: treeModel, specModel

    RowLayout {
        anchors.fill: parent
        spacing: 5

        // Левая панель – дерево
        Pane {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            padding: 0

            ColumnLayout {
                anchors.fill: parent
                spacing: 5

                Label {
                    text: "Дерево объектов"
                    font.bold: true
                    font.pointSize: 14
                    Layout.alignment: Qt.AlignHCenter
                }

                TreeView {
                    id: treeView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: treeModel

                    delegate: ItemDelegate {
                        width: parent.width
                        text: model.name
                        icon.source: model.iconSource
                        onClicked: {
                            if (model.type === "transmitter")
                                specModel.setTransmitterId(model.id)
                            else
                                specModel.setTransmitterId(-1)
                        }
                    }

                    // Раскрыть всё при загрузке
                    Component.onCompleted: expandAll()
                }

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: "Добавить объект"
                        Layout.fillWidth: true
                        onClicked: addObjectDialog.open()
                    }

                    Button {
                        text: "Добавить передатчик"
                        Layout.fillWidth: true
                        onClicked: {
                            var idx = treeView.currentIndex
                            if (idx.valid && treeModel.data(idx, ObjectTreeModel.TypeRole) === "object") {
                                addTransmitterDialog.objectId = treeModel.data(idx, ObjectTreeModel.IdRole)
                                addTransmitterDialog.open()
                            } else {
                                messageDialog.show("Выберите объект в дереве")
                            }
                        }
                    }

                    Button {
                        text: "Удалить"
                        Layout.fillWidth: true
                        onClicked: {
                            var idx = treeView.currentIndex
                            if (idx.valid) {
                                deleteConfirmDialog.index = idx
                                deleteConfirmDialog.open()
                            } else {
                                messageDialog.show("Выберите элемент для удаления")
                            }
                        }
                    }
                }
            }
        }

        // Правая панель – таблица ТТХ
        Pane {
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 0

            ColumnLayout {
                anchors.fill: parent
                spacing: 5

                Label {
                    text: "Таблица ТТХ"
                    font.bold: true
                    font.pointSize: 14
                    Layout.alignment: Qt.AlignHCenter
                }

                TableView {
                    id: tableView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: specModel
                    clip: true

                    columnWidthProvider: function(column) {
                        return column === 0 ? 150 : 100
                    }

                    delegate: Rectangle {
                        implicitHeight: 30
                        color: "transparent"

                        Text {
                            anchors.fill: parent
                            anchors.margins: 4
                            verticalAlignment: Text.AlignVCenter
                            text: display
                            visible: !editing
                        }

                        TextField {
                            anchors.fill: parent
                            anchors.margins: 2
                            text: display
                            visible: editing
                            validator: DoubleValidator { bottom: -1e9; top: 1e9; decimals: 2 }
                            onEditingFinished: {
                                model.display = text  // через роль display (setData)
                                editing = false
                            }
                            onVisibleChanged: if (visible) forceActiveFocus()
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (column === 1) {
                                    editing = true
                                }
                            }
                        }

                        property bool editing: false
                    }
                }
            }
        }
    }

    // Диалоги
    MessageDialog {
        id: messageDialog
        title: "Информация"
        icon: StandardIcon.Information
        standardButtons: StandardButton.Ok
        function show(msg) { text = msg; open() }
    }

    Dialog {
        id: addObjectDialog
        title: "Добавить объект"
        standardButtons: Dialog.Ok | Dialog.Cancel
        width: 300

        property string name
        property double lat
        property double lon
        property string iconPath

        contentItem: ColumnLayout {
            spacing: 8
            TextField {
                Layout.fillWidth: true
                placeholderText: "Название"
                onTextChanged: addObjectDialog.name = text
            }
            TextField {
                Layout.fillWidth: true
                placeholderText: "Широта"
                validator: DoubleValidator { bottom: -90; top: 90; decimals: 6 }
                onTextChanged: addObjectDialog.lat = parseFloat(text)
            }
            TextField {
                Layout.fillWidth: true
                placeholderText: "Долгота"
                validator: DoubleValidator { bottom: -180; top: 180; decimals: 6 }
                onTextChanged: addObjectDialog.lon = parseFloat(text)
            }
            RowLayout {
                Layout.fillWidth: true
                TextField {
                    id: iconField
                    Layout.fillWidth: true
                    placeholderText: "Путь к иконке"
                    readOnly: true
                    text: addObjectDialog.iconPath
                }
                Button {
                    text: "..."
                    onClicked: fileDialog.open()
                }
            }
        }

        onAccepted: {
            treeModel.addObject(name, lat, lon, iconPath)
        }
    }

    Dialog {
        id: addTransmitterDialog
        title: "Добавить передатчик"
        standardButtons: Dialog.Ok | Dialog.Cancel
        width: 300

        property int objectId: -1
        property string name
        property string iconPath

        contentItem: ColumnLayout {
            spacing: 8
            TextField {
                Layout.fillWidth: true
                placeholderText: "Название передатчика"
                onTextChanged: addTransmitterDialog.name = text
            }
            RowLayout {
                Layout.fillWidth: true
                TextField {
                    id: txIconField
                    Layout.fillWidth: true
                    placeholderText: "Путь к иконке"
                    readOnly: true
                    text: addTransmitterDialog.iconPath
                }
                Button {
                    text: "..."
                    onClicked: fileDialog.openForTransmitter()
                }
            }
        }

        onAccepted: {
            if (objectId !== -1 && name !== "")
                treeModel.addTransmitter(objectId, name, iconPath)
        }
    }

    FileDialog {
        id: fileDialog
        title: "Выберите иконку"
        nameFilters: ["Images (*.png *.jpg *.jpeg *.bmp *.svg)"]
        folder: shortcuts.pictures
        function openForTransmitter() {
            fileDialog.forObject = false
            fileDialog.open()
        }
        property bool forObject: true
        onAccepted: {
            var path = fileUrl.toString().replace("file:///", "")
            if (forObject) {
                addObjectDialog.iconPath = path
                iconField.text = path
            } else {
                addTransmitterDialog.iconPath = path
                txIconField.text = path
            }
        }
    }

    MessageDialog {
        id: deleteConfirmDialog
        title: "Подтверждение"
        icon: StandardIcon.Question
        standardButtons: StandardButton.Yes | StandardButton.No
        property var index
        function open(idx) { index = idx; super.open() }
        onYes: {
            treeModel.deleteItem(index)
            specModel.setTransmitterId(-1)
        }
    }
}