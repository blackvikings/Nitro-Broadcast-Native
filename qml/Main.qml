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

    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        SplitView {
            SplitView.fillWidth: true
            SplitView.fillHeight: true
            SplitView.minimumHeight: 320
            orientation: Qt.Horizontal

            ScenesPanel {
                SplitView.preferredWidth: 240
                SplitView.minimumWidth: 160
                SplitView.maximumWidth: 420
            }

            PreviewProgram {
                SplitView.fillWidth: true
                SplitView.minimumWidth: 400
            }

            SourcesPanel {
                SplitView.preferredWidth: 260
                SplitView.minimumWidth: 180
                SplitView.maximumWidth: 480
            }
        }

        MixerPanel {
            SplitView.preferredHeight: 180
            SplitView.minimumHeight: 120
            SplitView.maximumHeight: 320
        }
    }

    SettingsDialog {}
}
