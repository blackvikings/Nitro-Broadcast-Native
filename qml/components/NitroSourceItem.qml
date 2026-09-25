import QtQuick
import NitroBroadcast

Rectangle {
    id: root
    property string name: ""
    property string type: ""
    property bool enabled: true
    property bool locked: false
    property bool selected: false

    signal clicked()
    signal toggleEnabled()
    signal removeRequested()

    height: 34
    radius: Theme.radiusControl
    color: selected ? Theme.bgHover : "transparent"
    border.width: selected ? 1 : 0
    border.color: Theme.accent

    Row {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 8

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: enabled ? "◉" : "○"
            color: enabled ? Theme.success : Theme.textDisabled
            font.pixelSize: Theme.fontSm
            MouseArea {
                anchors.fill: parent
                onClicked: root.toggleEnabled()
            }
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - 50
            Text {
                width: parent.width
                text: root.name
                elide: Text.ElideRight
                color: Theme.textPrimary
                font.pixelSize: Theme.fontMd
            }
            Text {
                text: root.type
                color: Theme.textSecondary
                font.pixelSize: Theme.fontXs
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: locked ? "🔒" : ""
            font.pixelSize: Theme.fontSm
        }
    }

    MouseArea {
        anchors.fill: parent
        anchors.leftMargin: 28
        onClicked: root.clicked()
        onPressAndHold: root.removeRequested()
    }
}
