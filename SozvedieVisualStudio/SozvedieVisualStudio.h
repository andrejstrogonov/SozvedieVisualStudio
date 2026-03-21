// SozvedieVisualStudio.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.

#pragma once


// TODO: установите здесь ссылки на дополнительные заголовки, требующиеся для программы.
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    visible: true
    width: 1100
    height: 700
    title: "Data Management System (MVC + PostgreSQL)"

    FileDialog {
        id: iconDialog
        title: "Choose icon"
        currentFolder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
        nameFilters: ["Images (*.png *.svg *.jpg)"]
        onAccepted: {
            treeModel.updateIcon(treeView.currentIndex, fileUrl)
        }
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        ColumnLayout {
            SplitView.preferredWidth: 350
            spacing: 0

            Label {
                text: "Object Tree"
                font.pixelSize: 16
                font.bold: true
                Layout.margins: 10
            }

            ListView {
                id: treeView
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: treeModel
                delegate: TreeItemDelegate {
                    onUploadIcon: iconDialog.open()
                }
                highlight: Rectangle { color: "#e0e0e0" }
                focus: true
                onCurrentIndexChanged: treeModel.setCurrentIndex(currentIndex)
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.margins: 5
                Button {
                    text: "Add"
                    Layout.fillWidth: true
                    onClicked: treeModel.addNewItem()
                }
                Button {
                    text: "Delete"
                    Layout.fillWidth: true
                    onClicked: treeModel.removeItem(treeView.currentIndex)
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                height: 40
                color: "#f5f5f5"
                Label {
                    anchors.centerIn: parent
                    text: "Technical Characteristics"
                    font.bold: true
                }
            }

            TableView {
                id: ttxTable
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: ttxModel

                columnWidthProvider: function (column) {
                    return column === 0 ? 150 : 300
                }

                delegate: Rectangle {
                    implicitHeight: 40
                    border.color: "#eeeeee"

                    TextInput {
                        anchors.fill: parent
                        anchors.margins: 10
                        verticalAlignment: TextInput.AlignVCenter
                        text: model.display
                        readOnly: model.column === 0
                        color: model.column === 0 ? "#666666" : "#000000"

                        onEditingFinished: {
                            if (!readOnly) {
                                model.edit = text
                            }
                        }
                    }
                }
            }
        }
    }
}

// Компонент делегата для элементов дерева (определён один раз)
component TreeItemDelegate : ItemDelegate {
    id: root
    width: ListView.view.width
    text: model.display
    property string iconSource: model.icon
    signal uploadIcon()

    contentItem: RowLayout {
        spacing: 8
        Image {
            source: root.iconSource
            width: 24
            height: 24
            visible: source != ""
        }
        Label {
            text: root.text
            Layout.fillWidth: true
        }
        Button {
            text: "📷"
            onClicked: root.uploadIcon()
        }
    }
}