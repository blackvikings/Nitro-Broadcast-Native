import QtQuick
import QtQuick.Controls
import NitroBroadcast

Popup {
    id: root
    modal: true
    anchors.centerIn: Overlay.overlay
    width: 460
    height: 560
    visible: NitroState.settingsOpen
    onClosed: NitroState.settingsOpen = false
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle {
        color: Theme.bgElevated
        radius: Theme.radiusPanel
        border.width: 1
        border.color: Theme.borderSubtle
    }

    ScrollView {
        anchors.fill: parent
        anchors.margins: Theme.spaceLg
        clip: true

        Column {
            width: root.width - Theme.spaceLg * 2
            spacing: Theme.spaceMd

            Text {
                text: "Settings"
                color: Theme.textPrimary
                font.pixelSize: Theme.fontXl
                font.bold: true
            }

            Text { text: "Theme"; color: Theme.textSecondary; font.pixelSize: Theme.fontSm }
            ComboBox {
                width: parent.width
                model: ["dark", "light"]
                currentIndex: NitroSettings.theme === "light" ? 1 : 0
                onActivated: NitroSettings.theme = currentText
            }

            Text { text: "Resolution"; color: Theme.textSecondary; font.pixelSize: Theme.fontSm }
            ComboBox {
                width: parent.width
                model: ["1280x720", "1920x1080", "2560x1440", "3840x2160", "1080x1920"]
                Component.onCompleted: {
                    currentIndex = model.indexOf(NitroSettings.videoWidth + "x" + NitroSettings.videoHeight)
                    if (currentIndex < 0) currentIndex = 1
                }
                onActivated: {
                    const parts = currentText.split("x")
                    NitroSettings.videoWidth = Number(parts[0])
                    NitroSettings.videoHeight = Number(parts[1])
                }
            }

            Text { text: "Frame rate"; color: Theme.textSecondary; font.pixelSize: Theme.fontSm }
            ComboBox {
                width: parent.width
                model: ["30", "60"]
                currentIndex: NitroSettings.fps === 30 ? 0 : 1
                onActivated: NitroSettings.fps = Number(currentText)
            }

            Text { text: "Encoder"; color: Theme.textSecondary; font.pixelSize: Theme.fontSm }
            ComboBox {
                width: parent.width
                model: ["auto", "x264", "nvenc", "amf", "qsv"]
                Component.onCompleted: {
                    currentIndex = Math.max(0, model.indexOf(NitroSettings.encoder))
                }
                onActivated: NitroSettings.encoder = currentText
            }

            Rectangle {
                width: parent.width
                height: 1
                color: Theme.borderSubtle
            }

            Text {
                text: "AUDIO DEVICES"
                color: Theme.accent
                font.pixelSize: Theme.fontSm
                font.bold: true
            }

            Text { text: "Microphone"; color: Theme.textSecondary; font.pixelSize: Theme.fontSm }
            ComboBox {
                id: micBox
                width: parent.width
                model: NitroAudioDevices.inputDeviceNames()
                Component.onCompleted: syncMic()
                onActivated: {
                    const ids = NitroAudioDevices.inputDeviceIds()
                    if (currentIndex >= 0 && currentIndex < ids.length) {
                        NitroAudio.micDeviceId = ids[currentIndex]
                        NitroSettings.microphoneDeviceId = ids[currentIndex]
                    }
                }
                function syncMic() {
                    const ids = NitroAudioDevices.inputDeviceIds()
                    const idx = ids.indexOf(NitroAudio.micDeviceId)
                    currentIndex = idx >= 0 ? idx : 0
                }
            }

            Text { text: "Desktop Audio (loopback)"; color: Theme.textSecondary; font.pixelSize: Theme.fontSm }
            ComboBox {
                id: deskBox
                width: parent.width
                model: NitroAudioDevices.outputDeviceNames()
                Component.onCompleted: syncDesk()
                onActivated: {
                    const ids = NitroAudioDevices.outputDeviceIds()
                    if (currentIndex >= 0 && currentIndex < ids.length) {
                        NitroAudio.desktopDeviceId = ids[currentIndex]
                        NitroSettings.desktopDeviceId = ids[currentIndex]
                    }
                }
                function syncDesk() {
                    const ids = NitroAudioDevices.outputDeviceIds()
                    const idx = ids.indexOf(NitroAudio.desktopDeviceId)
                    currentIndex = idx >= 0 ? idx : 0
                }
            }

            Row {
                spacing: Theme.spaceSm
                NitroButton {
                    text: "Refresh Devices"
                    variant: "ghost"
                    onClicked: {
                        NitroAudioDevices.refresh()
                        micBox.model = NitroAudioDevices.inputDeviceNames()
                        deskBox.model = NitroAudioDevices.outputDeviceNames()
                        micBox.syncMic()
                        deskBox.syncDesk()
                    }
                }
                NitroButton {
                    text: NitroAudio.running ? "Restart Audio" : "Start Audio"
                    onClicked: {
                        if (NitroAudio.running)
                            NitroAudio.refreshAndRestart()
                        else
                            NitroAudio.start()
                    }
                }
            }

            Text {
                width: parent.width
                wrapMode: Text.WordWrap
                text: NitroAudio.statusText
                color: Theme.textDisabled
                font.pixelSize: Theme.fontXs
            }

            Text {
                width: parent.width
                wrapMode: Text.WordWrap
                text: NitroApp.ffmpegStatus
                color: Theme.textDisabled
                font.pixelSize: Theme.fontXs
            }

            NitroButton {
                anchors.right: parent.right
                text: "Close"
                variant: "ghost"
                onClicked: root.close()
            }
        }
    }
}
