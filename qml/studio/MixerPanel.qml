import QtQuick
import NitroBroadcast

NitroPanel {
    id: root
    title: "AUDIO MIXER"

    Row {
        anchors.fill: parent
        anchors.margins: Theme.spaceSm
        spacing: Theme.spaceMd

        ListView {
            id: list
            width: parent.width - 110
            height: parent.height
            orientation: ListView.Horizontal
            spacing: Theme.spaceMd
            clip: true
            model: NitroMixer

            delegate: Rectangle {
                width: 108
                height: list.height
                radius: Theme.radiusControl
                color: Theme.bgElevated
                border.width: 1
                border.color: Theme.borderSubtle

                Column {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    Text {
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        text: model.name
                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontXs
                        font.bold: true
                        font.letterSpacing: 0.8
                    }

                    Text {
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        text: model.sourceName
                        elide: Text.ElideMiddle
                        color: model.hasCapture ? Theme.textSecondary : Theme.textDisabled
                        font.pixelSize: 9
                    }

                    Item {
                        width: parent.width
                        height: 88
                        NitroMeter {
                            anchors.horizontalCenter: parent.horizontalCenter
                            level: model.level
                            peakHold: {
                                // Map peakHoldDb (-60..0) to 0..1 same as level
                                const db = model.peakHoldDb
                                return Math.max(0, Math.min(1, (db + 60) / 60))
                            }
                            simulated: false
                        }
                    }

                    Text {
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        text: model.hasCapture
                              ? (model.peakDb <= -119 ? "-∞ dB" : model.peakDb.toFixed(1) + " dB")
                              : "—"
                        color: Theme.textSecondary
                        font.pixelSize: 9
                        font.family: "Consolas"
                    }

                    NitroSlider {
                        width: parent.width
                        value: model.volume
                        onMoved: NitroMixer.setVolume(index, value)
                    }

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 4
                        NitroIconButton {
                            text: model.muted ? "M" : "🔊"
                            label: "Mute"
                            onClicked: NitroMixer.setMuted(index, !model.muted)
                        }
                        NitroIconButton {
                            text: model.monitoring ? "🎧" : "○"
                            label: "Monitor"
                            onClicked: NitroMixer.setMonitoring(index, !model.monitoring)
                        }
                        NitroIconButton {
                            text: "⚙"
                            label: "Channel settings"
                            onClicked: NitroState.settingsOpen = true
                        }
                    }
                }
            }
        }

        // Master bus
        Rectangle {
            width: 100
            height: parent.height
            radius: Theme.radiusControl
            color: Theme.bgElevated
            border.width: 1
            border.color: Theme.accent

            Column {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    text: "MASTER"
                    color: Theme.accent
                    font.pixelSize: Theme.fontXs
                    font.bold: true
                }

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    text: NitroAudio ? NitroAudio.statusText : ""
                    elide: Text.ElideRight
                    color: Theme.textDisabled
                    font.pixelSize: 8
                }

                Item {
                    width: parent.width
                    height: 88
                    NitroMeter {
                        anchors.horizontalCenter: parent.horizontalCenter
                        level: NitroMixer.masterLevel
                        peakHold: Math.max(0, Math.min(1, (NitroMixer.masterPeakDb + 60) / 60))
                        simulated: false
                    }
                }

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    text: NitroMixer.masterPeakDb <= -119
                          ? "-∞ dB"
                          : NitroMixer.masterPeakDb.toFixed(1) + " dB"
                    color: Theme.textSecondary
                    font.pixelSize: 9
                    font.family: "Consolas"
                }

                NitroSlider {
                    width: parent.width
                    value: (NitroMixer.masterGainDb + 60) / 72
                    onMoved: NitroMixer.masterGainDb = -60 + value * 72
                }

                NitroIconButton {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: NitroMixer.masterMuted ? "M" : "🔊"
                    label: "Master mute"
                    onClicked: NitroMixer.masterMuted = !NitroMixer.masterMuted
                }
            }
        }
    }
}
