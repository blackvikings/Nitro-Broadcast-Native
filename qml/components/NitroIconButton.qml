import QtQuick
import QtQuick.Controls
import NitroBroadcast

Button {
    id: root
    property string label: ""
    implicitWidth: 28
    implicitHeight: 28
    padding: 0
    ToolTip.visible: hovered && label.length > 0
    ToolTip.text: label
    ToolTip.delay: 400

    background: Rectangle {
        radius: Theme.radiusControl
        color: root.down ? Theme.bgHover : (root.hovered ? Theme.bgHover : "transparent")
        border.width: 1
        border.color: root.hovered ? Theme.borderSubtle : "transparent"
    }

    contentItem: Text {
        text: root.text
        color: Theme.textSecondary
        font.pixelSize: Theme.fontMd
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
