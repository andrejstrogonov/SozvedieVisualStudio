import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: "Добавить объект"
    width: 400
    height: 350
    anchors.centerIn: parent
    
    signal objectAdded(string name, real latitude, real longitude)
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10
        
        // Name field
        ColumnLayout {
            spacing: 5
            
            Text {
                text: "Имя объекта:"
                font.pixelSize: 11
                color: "#333333"
                font.bold: true
            }
            
            TextField {
                id: objectNameField
                Layout.fillWidth: true
                placeholderText: "Введите имя объекта"
                
                background: Rectangle {
                    color: "white"
                    border.color: objectNameField.focus ? "#0078d4" : "#cccccc"
                    border.width: 1
                    radius: 4
                }
            }
        }
        
        // Latitude field
        ColumnLayout {
            spacing: 5
            
            Text {
                text: "Широта (-90 до 90):"
                font.pixelSize: 11
                color: "#333333"
                font.bold: true
            }
            
            TextField {
                id: latitudeField
                Layout.fillWidth: true
                placeholderText: "Введите широту"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                
                background: Rectangle {
                    color: "white"
                    border.color: latitudeField.focus ? "#0078d4" : "#cccccc"
                    border.width: 1
                    radius: 4
                }
            }
            
            Text {
                text: "Ошибка: широта должна быть от -90 до 90"
                font.pixelSize: 10
                color: "#e81123"
                visible: {
                    if (latitudeField.text === "") return false
                    var val = parseFloat(latitudeField.text)
                    return isNaN(val) || val < -90 || val > 90
                }
            }
        }
        
        // Longitude field
        ColumnLayout {
            spacing: 5
            
            Text {
                text: "Долгота (-180 до 180):"
                font.pixelSize: 11
                color: "#333333"
                font.bold: true
            }
            
            TextField {
                id: longitudeField
                Layout.fillWidth: true
                placeholderText: "Введите долготу"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                
                background: Rectangle {
                    color: "white"
                    border.color: longitudeField.focus ? "#0078d4" : "#cccccc"
                    border.width: 1
                    radius: 4
                }
            }
            
            Text {
                text: "Ошибка: долгота должна быть от -180 до 180"
                font.pixelSize: 10
                color: "#e81123"
                visible: {
                    if (longitudeField.text === "") return false
                    var val = parseFloat(longitudeField.text)
                    return isNaN(val) || val < -180 || val > 180
                }
            }
        }
        
        Item { Layout.fillHeight: true }
        
        // Buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                Layout.fillWidth: true
                text: "Добавить"
                enabled: objectNameField.text !== "" && 
                         latitudeField.text !== "" && 
                         longitudeField.text !== ""
                
                onClicked: {
                    var lat = parseFloat(latitudeField.text)
                    var lon = parseFloat(longitudeField.text)
                    
                    if (lat >= -90 && lat <= 90 && lon >= -180 && lon <= 180) {
                        dialog.objectAdded(objectNameField.text, lat, lon)
                        objectNameField.text = ""
                        latitudeField.text = ""
                        longitudeField.text = ""
                        dialog.close()
                    }
                }
                
                background: Rectangle {
                    color: parent.pressed ? "#005a9e" : (parent.enabled ? "#0078d4" : "#cccccc")
                    radius: 4
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
                text: "Отмена"
                
                onClicked: {
                    objectNameField.text = ""
                    latitudeField.text = ""
                    longitudeField.text = ""
                    dialog.close()
                }
                
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
                    font.pixelSize: 11
                }
            }
        }
    }
}
