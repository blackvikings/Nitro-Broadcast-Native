import QtQuick
import QtQuick.Controls
import NitroBroadcast

Slider {
    id: root
    from: 0
    to: 1
    stepSize: 0.01
    implicitHeight: 18
    implicitWidth: 90

    background: Rectangle {
        x: root.leftPadding
        y: root.topPadding + root.availableHeight / 2 - height / 2
        implicitWidth: 90
        implicitHeight: 4
        width: root.availableWidth
        height: 4
        radius: 2
        color: Theme.bgInput

        Rectangle {
            width: root.visualPosition * parent.width
            height: parent.height
            color: Theme.accent
            radius: 2
        }
    }

    handle: Rectangle {
        x: root.leftPadding + root.visualPosition * (root.availableWidth - width)
        y: root.topPadding + root.availableHeight / 2 - height / 2
        width: 12
        height: 12
        radius: 6
        color: root.pressed ? Theme.accentPressed : Theme.textPrimary
        border.width: 1
        border.color: Theme.borderSubtle
    }
}
