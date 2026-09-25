import QtQuick
import QtQuick.Controls
import NitroBroadcast

NitroPanel {
    id: root
    title: "SOURCES"

    Item {
        anchors.fill: parent

        Row {
            id: actions
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 6
            spacing: 4
            z: 2

            NitroIconButton {
                text: "+"
                label: "Add source"
                onClicked: addMenu.open()
            }
            NitroIconButton {
                text: "−"
                label: "Remove source"
                onClicked: NitroScenes.removeSource(NitroScenes.selectedSourceIndex())
            }

            Menu {
                id: addMenu
                width: 300
                padding: 4

                Repeater {
                    model: NitroSourceCatalog.categoryNames()

                    delegate: Column {
                        width: addMenu.width - 8
                        property string catName: modelData

                        MenuSeparator { visible: index > 0; width: parent.width }

                        Text {
                            text: catName
                            color: Theme.textSecondary
                            font.pixelSize: Theme.fontXs
                            font.bold: true
                            leftPadding: 10
                            topPadding: 6
                            bottomPadding: 4
                        }

                        Repeater {
                            model: NitroSourceCatalog
                            delegate: Item {
                                width: addMenu.width - 8
                                height: model.categoryName === catName ? 28 : 0
                                visible: model.categoryName === catName

                                Rectangle {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    radius: 3
                                    color: itemMa.containsMouse ? Theme.bgHover : "transparent"
                                    opacity: model.canAdd ? 1.0 : 0.45

                                    Row {
                                        anchors.fill: parent
                                        anchors.leftMargin: 8
                                        spacing: 8
                                        NitroIcon {
                                            anchors.verticalCenter: parent.verticalCenter
                                            name: model.icon
                                            iconColor: model.canAdd ? Theme.textPrimary : Theme.textDisabled
                                        }
                                        Text {
                                            anchors.verticalCenter: parent.verticalCenter
                                            text: model.displayName
                                            color: model.canAdd ? Theme.textPrimary : Theme.textDisabled
                                            font.pixelSize: Theme.fontSm
                                        }
                                        Rectangle {
                                            visible: model.capability === "Experimental"
                                            anchors.verticalCenter: parent.verticalCenter
                                            width: 28; height: 14; radius: 2
                                            color: Theme.warning
                                            Text {
                                                anchors.centerIn: parent
                                                text: "EXP"
                                                font.pixelSize: 9
                                                color: "#111"
                                            }
                                        }
                                        Text {
                                            visible: !model.canAdd
                                            anchors.verticalCenter: parent.verticalCenter
                                            text: model.capabilityLabel
                                            color: Theme.textDisabled
                                            font.pixelSize: Theme.fontXs
                                        }
                                    }

                                    MouseArea {
                                        id: itemMa
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        enabled: model.canAdd
                                        onClicked: {
                                            NitroScenes.addSource(model.typeKey)
                                            addMenu.close()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        ListView {
            id: list
            anchors.fill: parent
            anchors.topMargin: 36
            anchors.bottomMargin: propsPanel.visible ? propsPanel.height + 4 : 4
            anchors.margins: 6
            clip: true
            spacing: 4
            model: NitroSources

            delegate: NitroSourceItem {
                width: list.width
                name: model.name
                type: model.typeName + (model.capability !== "Available" ? " · " + model.capability : "")
                enabled: model.enabled
                locked: model.locked
                selected: model.selected
                onClicked: NitroScenes.selectSource(index)
                onToggleEnabled: NitroScenes.setSourceEnabled(index, !model.enabled)
                onRemoveRequested: NitroScenes.removeSource(index)
            }
        }

        Rectangle {
            id: propsPanel
            visible: NitroSourceProps.hasSelection
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: visible ? Math.min(200, root.height * 0.42) : 0
            color: Theme.bgPanel
            border.width: 1
            border.color: Theme.borderSubtle

            Flickable {
                anchors.fill: parent
                anchors.margins: 8
                contentHeight: propsCol.height
                clip: true
                Column {
                    id: propsCol
                    width: parent.width
                    spacing: 6
                    Text {
                        text: NitroSourceProps.typeName
                        color: Theme.accent
                        font.pixelSize: Theme.fontXs
                        font.bold: true
                    }
                    Text {
                        width: parent.width
                        text: NitroSourceProps.capabilityNote
                        color: Theme.textSecondary
                        font.pixelSize: Theme.fontXs
                        wrapMode: Text.WordWrap
                        visible: text.length > 0
                    }
                    TextField {
                        width: parent.width
                        text: NitroSourceProps.name
                        onEditingFinished: NitroSourceProps.name = text
                    }
                    Row {
                        spacing: 4
                        NitroIconButton { text: "↺"; label: "Reset"; onClicked: NitroSourceProps.resetTransform() }
                        NitroIconButton { text: "▣"; label: "Fit"; onClicked: NitroSourceProps.fitToCanvas() }
                        NitroIconButton { text: "⊕"; label: "Center"; onClicked: NitroSourceProps.center() }
                        NitroIconButton { text: "↔"; label: "Stretch"; onClicked: NitroSourceProps.stretch() }
                    }
                }
            }
        }
    }
}
