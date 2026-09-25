import QtQuick
import NitroBroadcast

Rectangle {
    id: root
    property string text: "Idle"
    property color tone: Theme.textSecondary
    property bool active: false

    implicitHeight: 22
    implicitWidth: row.width + 14
    radius: 11
    color: active ? Qt.rgba(tone.r, tone.g, tone.b, 0.18) : Theme.bgInput
    border.width: 1
    border.color: active ? tone : Theme.borderSubtle

    Row {
        id: row
        anchors.centerIn: parent
        spacing: 6

        Rectangle {
            width: 6
            height: 6
            radius: 3
            anchors.verticalCenter: parent.verticalCenter
            color: root.active ? root.tone : Theme.textDisabled
        }

        Text {
            text: root.text
            color: root.active ? root.tone : Theme.textSecondary
            font.pixelSize: Theme.fontXs
            font.family: "Segoe UI"
        }
    }
}
