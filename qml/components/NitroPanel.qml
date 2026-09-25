import QtQuick
import NitroBroadcast

Rectangle {
    id: root
    property string title: ""
    default property alias content: body.data

    color: Theme.bgPanel
    radius: Theme.radiusPanel
    border.width: 1
    border.color: Theme.borderSubtle

    Column {
        anchors.fill: parent
        anchors.margins: 1

        Rectangle {
            width: parent.width
            height: title.length > 0 ? 32 : 0
            color: Theme.bgElevated
            visible: title.length > 0

            Text {
                anchors.left: parent.left
                anchors.leftMargin: Theme.spaceSm
                anchors.verticalCenter: parent.verticalCenter
                text: root.title
                color: Theme.textSecondary
                font.pixelSize: Theme.fontSm
                font.bold: true
                font.letterSpacing: 0.6
            }
        }

        Item {
            id: body
            width: parent.width
            height: parent.height - (title.length > 0 ? 32 : 0)
        }
    }
}
