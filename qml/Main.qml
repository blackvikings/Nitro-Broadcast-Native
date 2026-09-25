import QtQuick
import QtQuick.Controls
import QtQuick.Window
import NitroBroadcast

ApplicationWindow {
    id: window
    width: 1440
    height: 900
    minimumWidth: 1100
    minimumHeight: 700
    visible: true
    title: "Nitro Broadcast"
    color: Theme.bgApp

    header: TopBar {
        width: window.width
    }

    footer: StatusBar {
        width: window.width
    }

    Column {
        anchors.fill: parent
        spacing: 0

        Row {
            width: parent.width
            height: parent.height - 180

            ScenesPanel {
                width: 240
                height: parent.height
            }

            Rectangle {
                width: 1
                height: parent.height
                color: Theme.borderSubtle
            }

            PreviewProgram {
                width: parent.width - 480
                height: parent.height
            }

            Rectangle {
                width: 1
                height: parent.height
                color: Theme.borderSubtle
            }

            SourcesPanel {
                width: 240
                height: parent.height
            }
        }

        Rectangle {
            width: parent.width
            height: 1
            color: Theme.borderSubtle
        }

        MixerPanel {
            width: parent.width
            height: 180
        }
    }

    SettingsDialog {}
}
