import QtQuick
import QtQuick.Controls

ItemDelegate {
    id: delegate
    
    required property TreeView treeView
    required property bool isTreeNode
    required property bool expanded
    
    implicitWidth: treeView.width
    implicitHeight: 30
    
    contentItem: RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 5
        spacing: 5
        
        // Expand/collapse indicator
        Image {
            width: 16
            height: 16
            source: delegate.expanded ? "qrc:/icons/expanded.png" : "qrc:/icons/collapsed.png"
            visible: delegate.isTreeNode
            
            MouseArea {
                anchors.fill: parent
                onClicked: delegate.treeView.toggleExpanded(delegate.row)
            }
        }
        
        // Item icon
        Rectangle {
            width: 20
            height: 20
            radius: 3
            color: delegate.selected ? "#0078d4" : "#e8e8e8"
            
            Text {
                anchors.centerIn: parent
                text: delegate.isTreeNode ? "📁" : "📡"
                font.pixelSize: 12
            }
        }
        
        // Item name
        Text {
            text: model.display
            Layout.fillWidth: true
            verticalAlignment: Text.AlignVCenter
            color: delegate.selected ? "white" : "#333333"
            font.pixelSize: 11
        }
        
        // Coordinates (if available)
        Text {
            text: model.coordinates || ""
            Layout.fillWidth: true
            verticalAlignment: Text.AlignVCenter
            color: delegate.selected ? "white" : "#666666"
            font.pixelSize: 10
            visible: model.coordinates !== undefined
        }
    }
    
    background: Rectangle {
        color: delegate.selected ? "#0078d4" : (delegate.hovered ? "#e8e8e8" : "transparent")
        radius: 2
    }
}
