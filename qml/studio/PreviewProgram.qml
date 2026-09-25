import QtQuick
import NitroBroadcast

Item {
    id: root

    Row {
        anchors.fill: parent
        anchors.margins: Theme.spaceSm
        spacing: Theme.spaceSm

        // Preview
        Column {
            width: (parent.width - 72 - Theme.spaceSm * 2) / 2
            height: parent.height
            spacing: 6

            Text {
                text: "PREVIEW"
                color: Theme.preview
                font.pixelSize: Theme.fontSm
                font.bold: true
                font.letterSpacing: 1.0
            }

            Rectangle {
                width: parent.width
                height: parent.height - 24
                radius: Theme.radiusPanel
                color: "#050607"
                border.width: 2
                border.color: Theme.preview

                Column {
                    anchors.centerIn: parent
                    spacing: 8
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: NitroScenes.previewSceneName
                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontLg
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: NitroSettings.videoWidth + "×" + NitroSettings.videoHeight
                              + " @" + NitroSettings.fps
                        color: Theme.textSecondary
                        font.pixelSize: Theme.fontSm
                        font.family: "Consolas"
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "No media engine preview yet"
                        color: Theme.textDisabled
                        font.pixelSize: Theme.fontXs
                    }
                }
            }
        }

        // Transitions
        Column {
            width: 72
            anchors.verticalCenter: parent.verticalCenter
            spacing: 8

            NitroButton {
                width: parent.width
                text: "CUT"
                onClicked: NitroScenes.cutToProgram()
            }
            NitroButton {
                width: parent.width
                text: "FADE"
                variant: "ghost"
                onClicked: NitroScenes.fadeToProgram()
            }
            NitroButton {
                width: parent.width
                text: "FX"
                variant: "ghost"
                enabled: false
            }
        }

        // Program
        Column {
            width: (parent.width - 72 - Theme.spaceSm * 2) / 2
            height: parent.height
            spacing: 6

            Text {
                text: "PROGRAM"
                color: Theme.program
                font.pixelSize: Theme.fontSm
                font.bold: true
                font.letterSpacing: 1.0
            }

            Rectangle {
                width: parent.width
                height: parent.height - 24
                radius: Theme.radiusPanel
                color: "#050607"
                border.width: 2
                border.color: Theme.program

                Column {
                    anchors.centerIn: parent
                    spacing: 8
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: NitroScenes.programSceneName
                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontLg
                    }
                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: liveRow.width + 16
                        height: 22
                        radius: 11
                        color: NitroState.streamingUiActive ? Qt.rgba(Theme.program.r, Theme.program.g, Theme.program.b, 0.2) : Theme.bgInput
                        border.color: Theme.program
                        border.width: 1
                        Row {
                            id: liveRow
                            anchors.centerIn: parent
                            spacing: 6
                            Rectangle {
                                width: 6; height: 6; radius: 3
                                color: NitroState.streamingUiActive ? Theme.program : Theme.textDisabled
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text {
                                text: NitroState.streamingUiActive ? "ON AIR (UI)" : "PROGRAM"
                                color: Theme.program
                                font.pixelSize: Theme.fontXs
                            }
                        }
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "No media engine output yet"
                        color: Theme.textDisabled
                        font.pixelSize: Theme.fontXs
                    }
                }
            }
        }
    }
}
