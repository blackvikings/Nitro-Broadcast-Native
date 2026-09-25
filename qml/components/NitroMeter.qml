import QtQuick
import NitroBroadcast

Item {
    id: root
    property real level: 0.0
    property real peakHold: 0.0
    property bool simulated: false

    width: 10
    height: 88

    Rectangle {
        anchors.fill: parent
        radius: 2
        color: Theme.bgInput
        border.width: 1
        border.color: Theme.borderSubtle

        // Peak / RMS bar
        Rectangle {
            id: bar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: parent.height * Math.max(0, Math.min(1, root.level))
            radius: 2
            color: root.level > 0.92 ? Theme.meterClip
                 : root.level > 0.7 ? Theme.meterWarn
                 : Theme.meterOk
        }

        // Peak hold tick
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            height: 2
            y: parent.height * (1.0 - Math.max(0, Math.min(1, root.peakHold))) - 1
            visible: root.peakHold > 0.02
            color: Theme.textPrimary
            opacity: 0.85
        }
    }
}
