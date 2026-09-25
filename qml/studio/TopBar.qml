import QtQuick
import NitroBroadcast

Rectangle {
    id: root
    height: 48
    color: Theme.bgPanel
    border.width: 1
    border.color: Theme.borderSubtle

    Row {
        anchors.fill: parent
        anchors.leftMargin: Theme.spaceMd
        anchors.rightMargin: Theme.spaceMd
        spacing: Theme.spaceLg

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "NITRO BROADCAST"
            color: Theme.accent
            font.pixelSize: Theme.fontTitle
            font.bold: true
            font.letterSpacing: 1.2
        }

        Item { width: 12; height: 1 }

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: profileRow.width + 16
            height: 28
            radius: 4
            color: Theme.bgInput
            border.width: 1
            border.color: Theme.borderSubtle

            Row {
                id: profileRow
                anchors.centerIn: parent
                spacing: 6
                Text {
                    text: "PROFILE"
                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontXs
                }
                Text {
                    text: NitroSettings.currentProfile
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSm
                }
            }
        }

        Item { width: Math.max(24, root.width - 820); height: 1 }

        NitroStatusBadge {
            anchors.verticalCenter: parent.verticalCenter
            text: NitroState.streamStatusText
            tone: Theme.program
            active: NitroState.streamingUiActive
        }

        NitroStatusBadge {
            anchors.verticalCenter: parent.verticalCenter
            text: NitroState.recordStatusText
            tone: Theme.danger
            active: NitroState.recordingUiActive
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "CPU " + NitroState.cpuUsage.toFixed(0) + "%"
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSm
            font.family: "Consolas"
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "GPU " + NitroState.gpuUsage.toFixed(0) + "%"
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSm
            font.family: "Consolas"
        }

        NitroIconButton {
            anchors.verticalCenter: parent.verticalCenter
            text: "⚙"
            label: "Settings"
            onClicked: NitroState.settingsOpen = true
        }

        NitroIconButton {
            anchors.verticalCenter: parent.verticalCenter
            text: "👤"
            label: "Account"
        }
    }
}
