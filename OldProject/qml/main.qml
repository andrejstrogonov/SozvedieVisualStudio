import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    id: root
    visible: true
    width: 1200
    height: 700
    title: "Система управления объектами и характеристиками"

    color: "#f5f5f5"

    // Main layout
    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // Left panel - Tree view
        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: parent.width / 2

            // Title
            Text {
                text: "Навигация по объектам"
                font.bold: true
                font.pixelSize: 12
                color: "#333333"
            }

            // Tree view
            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "white"
                border.color: "#cccccc"
                border.width: 1
                radius: 4

                TreeView {
                    id: treeView
                    anchors.fill: parent
                    anchors.margins: 5

                    delegate: TreeViewDelegate {
                        id: delegate
                        implicitWidth: treeView.width
                        implicitHeight: 30

                        contentItem: RowLayout {
                            spacing: 5
                            anchors.fill: parent
                            anchors.leftMargin: 5

                            Text {
                                text: model.display
                                Layout.fillWidth: true
                                verticalAlignment: Text.AlignVCenter
                                color: delegate.selected ? "white" : "#333333"
                            }

                            Text {
                                text: model.coordinates || ""
                                Layout.fillWidth: true
                                verticalAlignment: Text.AlignVCenter
                                color: delegate.selected ? "white" : "#666666"
                                font.pixelSize: 10
                            }
                        }

                        background: Rectangle {
                            color: delegate.selected ? "#0078d4" : (delegate.hovered ? "#e8e8e8" : "transparent")
                            radius: 2
                        }

                        onClicked: {
                            treeView.currentIndex = index
                            postgresApp.onTreeSelectionChanged(model)
                        }
                    }
                }
            }

            // Control buttons
            RowLayout {
                Layout.fillWidth: true
                spacing: 5

                Button {
                    Layout.fillWidth: true
                    text: "Добавить объект"
                    onClicked: addObjectDialog.open()

                    background: Rectangle {
                        color: parent.pressed ? "#005a9e" : "#0078d4"
                        radius: 4
                        border.color: "#0078d4"
                        border.width: 1
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 11
                    }
                }

                Button {
                    Layout.fillWidth: true
                    text: "Добавить передачу"
                    onClicked: addTransmitterDialog.open()

                    background: Rectangle {
                        color: parent.pressed ? "#005a9e" : "#0078d4"
                        radius: 4
                        border.color: "#0078d4"
                        border.width: 1
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 11
                    }
                }

                Button {
                    Layout.fillWidth: true
                    text: "Удалить элемент"
                    onClicked: postgresApp.deleteElement()

                    background: Rectangle {
                        color: parent.pressed ? "#c50f1f" : "#e81123"
                        radius: 4
                        border.color: "#e81123"
                        border.width: 1
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 11
                    }
                }
            }
        }

        // Right panel - Table view
        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: parent.width / 2

            // Title
            Text {
                text: "Технические характеристики"
                font.bold: true
                font.pixelSize: 12
                color: "#333333"
            }

            // Table view
            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "white"
                border.color: "#cccccc"
                border.width: 1
                radius: 4

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 5
                    spacing: 5

                    // Header
                    RowLayout {
                        Layout.fillWidth: true
                        height: 30
                        spacing: 5

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredWidth: parent.width / 2
                            height: 30
                            color: "#f0f0f0"
                            border.color: "#cccccc"
                            border.width: 1
                            radius: 2

                            Text {
                                anchors.centerIn: parent
                                text: "Параметр"
                                font.bold: true
                                color: "#333333"
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredWidth: parent.width / 2
                            height: 30
                            color: "#f0f0f0"
                            border.color: "#cccccc"
                            border.width: 1
                            radius: 2

                            Text {
                                anchors.centerIn: parent
                                text: "Значение"
                                font.bold: true
                                color: "#333333"
                            }
                        }
                    }

                    // Specs list
                    ListView {
                        id: specsListView
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        spacing: 5
                        clip: true

                        model: postgresApp.specsModel

                        delegate: RowLayout {
                            width: specsListView.width
                            height: 40
                            spacing: 5

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredWidth: parent.width / 2
                                height: 40
                                color: "white"
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 2

                                Text {
                                    anchors.centerIn: parent
                                    text: model.paramName
                                    color: "#333333"
                                    font.pixelSize: 11
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredWidth: parent.width / 2
                                height: 40
                                color: "white"
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 2

                                TextInput {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    text: model.paramValue
                                    verticalAlignment: TextInput.AlignVCenter
                                    color: "#333333"
                                    font.pixelSize: 11

                                    onEditingFinished: {
                                        postgresApp.updateSpec(model.index, text)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Dialogs
    Dialog {
        id: addObjectDialog
        title: "Добавить объект"
        width: 400
        height: 300
        anchors.centerIn: parent

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 10

            Text {
                text: "Имя объекта:"
                font.pixelSize: 11
                color: "#333333"
            }

            TextField {
                id: objectNameField
                Layout.fillWidth: true
                placeholderText: "Введите имя объекта"
                background: Rectangle {
                    color: "white"
                    border.color: "#cccccc"
                    border.width: 1
                    radius: 4
                }
            }

            Text {
                text: "Широта:"
                font.pixelSize: 11
                color: "#333333"
            }

            TextField {
                id: latitudeField
                Layout.fillWidth: true
                placeholderText: "Введите широту (-90 до 90)"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                background: Rectangle {
                    color: "white"
                    border.color: "#cccccc"
                    border.width: 1
                    radius: 4
                }
            }

            Text {
                text: "Долгота:"
                font.pixelSize: 11
                color: "#333333"
            }

            TextField {
                id: longitudeField
                Layout.fillWidth: true
                placeholderText: "Введите долготу (-180 до 180)"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                background: Rectangle {
                    color: "white"
                    border.color: "#cccccc"
                    border.width: 1
                    radius: 4
                }
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Button {
                    Layout.fillWidth: true
                    text: "Добавить"
                    onClicked: {
                        postgresApp.addObject(
                            objectNameField.text,
                            parseFloat(latitudeField.text),
                            parseFloat(longitudeField.text)
                        )
                        objectNameField.text = ""
                        latitudeField.text = ""
                        longitudeField.text = ""
                        addObjectDialog.close()
                    }

                    background: Rectangle {
                        color: parent.pressed ? "#005a9e" : "#0078d4"
                        radius: 4
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    Layout.fillWidth: true
                    text: "Отмена"
                    onClicked: addObjectDialog.close()

                    background: Rectangle {
                        color: parent.pressed ? "#f3f3f3" : "#ffffff"
                        border.color: "#cccccc"
                        border.width: 1
                        radius: 4
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "#333333"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }

    Dialog {
        id: addTransmitterDialog
        title: "Добавить передачу"
        width: 400
        height: 200
        anchors.centerIn: parent

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 10

            Text {
                text: "Имя передачи:"
                font.pixelSize: 11
                color: "#333333"
            }

            TextField {
                id: transmitterNameField
                Layout.fillWidth: true
                placeholderText: "Введите имя передачи"
                background: Rectangle {
                    color: "white"
                    border.color: "#cccccc"
                    border.width: 1
                    radius: 4
                }
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Button {
                    Layout.fillWidth: true
                    text: "Добавить"
                    onClicked: {
                        postgresApp.addTransmitter(transmitterNameField.text)
                        transmitterNameField.text = ""
                        addTransmitterDialog.close()
                    }

                    background: Rectangle {
                        color: parent.pressed ? "#005a9e" : "#0078d4"
                        radius: 4
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    Layout.fillWidth: true
                    text: "Отмена"
                    onClicked: addTransmitterDialog.close()

                    background: Rectangle {
                        color: parent.pressed ? "#f3f3f3" : "#ffffff"
                        border.color: "#cccccc"
                        border.width: 1
                        radius: 4
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "#333333"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
}
