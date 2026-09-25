import QtQuick
import QtQuick.Controls
import NitroBroadcast

Rectangle {
    id: root
    property string name: ""
    property bool isPreview: false
    property bool isProgram: false
    property bool selected: isPreview

    signal clicked()
    signal renameRequested()
    signal deleteRequested()
    signal duplicateRequested()

    height: 34
    radius: Theme.radiusControl
    color: selected ? Theme.bgHover : "transparent"
    border.width: selected ? 1 : 0
    border.color: Theme.accent

    Row {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 6
        spacing: 8

        Rectangle {
            width: 8
            height: 8
            radius: 4
            anchors.verticalCenter: parent.verticalCenter
            color: isProgram ? Theme.program : "transparent"
            border.width: isProgram ? 0 : 1
            border.color: Theme.borderSubtle
            visible: true
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - 70
            text: root.name
            elide: Text.ElideRight
            color: Theme.textPrimary
            font.pixelSize: Theme.fontMd
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: function(mouse) {
            if (mouse.button === Qt.RightButton) {
                menu.open()
            } else {
                root.clicked()
            }
        }
        onDoubleClicked: root.renameRequested()
    }

    Menu {
        id: menu
        MenuItem { text: "Rename"; onTriggered: root.renameRequested() }
        MenuItem { text: "Duplicate"; onTriggered: root.duplicateRequested() }
        MenuItem { text: "Delete"; onTriggered: root.deleteRequested() }
    }
}
