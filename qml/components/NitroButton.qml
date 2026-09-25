import QtQuick
import QtQuick.Controls
import NitroBroadcast

Button {
    id: root

    property string variant: "primary" // primary | ghost | danger

    implicitHeight: 32
    padding: 12
    font.pixelSize: Theme.fontMd
    font.family: "Segoe UI"

    background: Rectangle {
        radius: Theme.radiusControl
        color: {
            if (!root.enabled) return Theme.bgHover
            if (root.variant === "danger")
                return root.down ? "#C84C62" : (root.hovered ? "#F48790" : Theme.danger)
            if (root.variant === "ghost")
                return root.down ? Theme.bgHover : (root.hovered ? Theme.bgHover : "transparent")
            return root.down ? Theme.accentPressed : (root.hovered ? "#6C9AFF" : Theme.accent)
        }
        border.width: root.variant === "ghost" ? 1 : 0
        border.color: Theme.borderSubtle
    }

    contentItem: Text {
        text: root.text
        color: {
            if (!root.enabled) return Theme.textDisabled
            if (root.variant === "ghost") return Theme.textPrimary
            if (root.variant === "danger") return "#12080A"
            return "#0B1020"
        }
        font: root.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
