import QtQuick
import NitroBroadcast

Rectangle {
    id: root
    height: 52
    color: Theme.bgPanel
    border.width: 1
    border.color: Theme.borderSubtle

    Row {
        anchors.left: parent.left
        anchors.leftMargin: Theme.spaceMd
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.spaceLg

        Text {
            text: NitroSettings.videoWidth + "x" + NitroSettings.videoHeight
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSm
            font.family: "Consolas"
        }
        Text {
            text: NitroSettings.fps + " FPS"
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSm
            font.family: "Consolas"
        }
        Text {
            text: NitroState.bitrateKbps + " kbps"
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSm
            font.family: "Consolas"
        }
        Text {
            text: "Drop " + NitroState.droppedFrames
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSm
            font.family: "Consolas"
        }
        Text {
            text: "Enc " + NitroState.encoderName
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSm
            font.family: "Consolas"
        }
        Text {
            text: NitroState.metricsSimulated ? "CPU " + NitroState.cpuUsage.toFixed(0) + "% (sim)" : "CPU " + NitroState.cpuUsage.toFixed(0) + "%"
            color: NitroState.metricsSimulated ? Theme.warning : Theme.textSecondary
            font.pixelSize: Theme.fontSm
            font.family: "Consolas"
        }
        Text {
            text: NitroState.metricsSimulated ? "GPU " + NitroState.gpuUsage.toFixed(0) + "% (sim)" : "GPU " + NitroState.gpuUsage.toFixed(0) + "%"
            color: NitroState.metricsSimulated ? Theme.warning : Theme.textSecondary
            font.pixelSize: Theme.fontSm
            font.family: "Consolas"
        }
        Text {
            text: "Net " + NitroState.networkStatus + (NitroState.metricsSimulated ? " (sim)" : "")
            color: NitroState.metricsSimulated ? Theme.warning : Theme.textSecondary
            font.pixelSize: Theme.fontSm
            font.family: "Consolas"
        }
    }

    Row {
        anchors.right: parent.right
        anchors.rightMargin: Theme.spaceMd
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.spaceSm

        NitroButton {
            text: NitroState.recordingUiActive ? "STOP REC" : "RECORD"
            variant: NitroState.recordingUiActive ? "danger" : "ghost"
            onClicked: NitroState.toggleRecordingSimulation()
        }
        NitroButton {
            text: NitroState.streamingUiActive ? "STOP STREAM" : "START STREAM"
            variant: NitroState.streamingUiActive ? "danger" : "primary"
            onClicked: NitroState.toggleStreamingSimulation()
        }
    }
}
