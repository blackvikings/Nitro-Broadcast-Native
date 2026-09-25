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
                width: 280
                padding: 4

                Repeater {
                    model: NitroSourceCatalog.categoryNames()

                    delegate: Column {
                        width: addMenu.width - 8
                        property string catName: modelData

                        MenuSeparator {
                            visible: index > 0
                            width: parent.width
                        }

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

                            delegate: MenuItem {
                                visible: model.categoryName === catName
                                height: visible ? implicitHeight : 0
                                width: addMenu.width - 8
                                text: model.icon + "  " + model.displayName
                                      + (model.capability === "Available" ? "" : "  ·  " + model.capabilityLabel)
                                opacity: (model.capability === "Available" || model.capability === "Experimental") ? 1.0 : 0.55
                                onTriggered: NitroScenes.addSource(model.typeKey)
                                ToolTip.visible: hovered && model.note.length > 0
                                ToolTip.text: model.note
                                ToolTip.delay: 400
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

        // Compact properties strip for selected source
        Rectangle {
            id: propsPanel
            visible: NitroSourceProps.hasSelection
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: visible ? Math.min(220, root.height * 0.45) : 0
            color: Theme.bgPanel
            border.width: 1
            border.color: Theme.borderSubtle

            Flickable {
                anchors.fill: parent
                anchors.margins: 8
                contentHeight: propsCol.height
                clip: true
                flickableDirection: Flickable.VerticalFlick

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
                        color: Theme.textPrimary
                        placeholderText: "Name"
                        onEditingFinished: NitroSourceProps.name = text
                    }

                    // Transform row
                    Grid {
                        width: parent.width
                        columns: 2
                        columnSpacing: 6
                        rowSpacing: 4

                        Text { text: "X"; color: Theme.textSecondary; font.pixelSize: Theme.fontXs }
                        SpinBox {
                            from: -10000; to: 10000
                            value: Math.round(NitroSourceProps.x)
                            editable: true
                            onValueModified: NitroSourceProps.x = value
                        }
                        Text { text: "Y"; color: Theme.textSecondary; font.pixelSize: Theme.fontXs }
                        SpinBox {
                            from: -10000; to: 10000
                            value: Math.round(NitroSourceProps.y)
                            editable: true
                            onValueModified: NitroSourceProps.y = value
                        }
                        Text { text: "W"; color: Theme.textSecondary; font.pixelSize: Theme.fontXs }
                        SpinBox {
                            from: 0; to: 10000
                            value: Math.round(NitroSourceProps.width)
                            editable: true
                            onValueModified: NitroSourceProps.width = value
                        }
                        Text { text: "H"; color: Theme.textSecondary; font.pixelSize: Theme.fontXs }
                        SpinBox {
                            from: 0; to: 10000
                            value: Math.round(NitroSourceProps.height)
                            editable: true
                            onValueModified: NitroSourceProps.height = value
                        }
                    }

                    Row {
                        spacing: 4
                        NitroIconButton { text: "↺"; label: "Reset"; onClicked: NitroSourceProps.resetTransform() }
                        NitroIconButton { text: "▣"; label: "Fit"; onClicked: NitroSourceProps.fitToCanvas() }
                        NitroIconButton { text: "⊕"; label: "Center"; onClicked: NitroSourceProps.center() }
                        NitroIconButton { text: "↔"; label: "Stretch"; onClicked: NitroSourceProps.stretch() }
                    }

                    // Type-specific quick props
                    Loader {
                        width: parent.width
                        sourceComponent: {
                            var t = NitroSourceProps.typeName
                            if (t === "Display Capture") return displayProps
                            if (t === "Window Capture") return windowProps
                            if (t === "Camera") return cameraProps
                            if (t === "Image") return imageProps
                            if (t === "Text") return textProps
                            if (t === "Color Source") return colorProps
                            if (t.indexOf("Audio") >= 0) return audioProps
                            return genericProps
                        }
                    }
                }
            }
        }
    }

    Component {
        id: displayProps
        Column {
            width: parent ? parent.width : 200
            spacing: 4
            Text { text: "Monitor / FPS"; color: Theme.textSecondary; font.pixelSize: Theme.fontXs }
            SpinBox {
                from: 0; to: 15
                value: NitroSourceProps.properties.monitorIndex || 0
                onValueModified: NitroSourceProps.setPropertyValue("monitorIndex", value)
            }
            SpinBox {
                from: 1; to: 240
                value: NitroSourceProps.properties.fps || 60
                onValueModified: NitroSourceProps.setPropertyValue("fps", value)
            }
            Text {
                text: "API: " + (NitroCapture ? NitroCapture.preferredApi : "WGC")
                      + " · " + (NitroCapture ? NitroCapture.monitors().length : 0) + " monitors"
                color: Theme.textSecondary
                font.pixelSize: Theme.fontXs
            }
        }
    }

    Component {
        id: windowProps
        Column {
            spacing: 4
            TextField {
                width: parent ? parent.width : 200
                placeholderText: "Window title"
                text: NitroSourceProps.properties.windowTitle || ""
                onEditingFinished: NitroSourceProps.setPropertyValue("windowTitle", text)
            }
            Text {
                text: (NitroCapture ? NitroCapture.windows().length : 0) + " windows enumerated"
                color: Theme.textSecondary
                font.pixelSize: Theme.fontXs
            }
        }
    }

    Component {
        id: cameraProps
        Column {
            spacing: 4
            TextField {
                width: parent ? parent.width : 200
                placeholderText: "Device ID"
                text: NitroSourceProps.properties.deviceId || ""
                onEditingFinished: NitroSourceProps.setPropertyValue("deviceId", text)
            }
            TextField {
                width: parent ? parent.width : 200
                placeholderText: "Resolution e.g. 1280x720"
                text: NitroSourceProps.properties.resolution || ""
                onEditingFinished: NitroSourceProps.setPropertyValue("resolution", text)
            }
        }
    }

    Component {
        id: imageProps
        TextField {
            width: parent ? parent.width : 200
            placeholderText: "Image file path"
            text: NitroSourceProps.properties.filePath || ""
            onEditingFinished: NitroSourceProps.setPropertyValue("filePath", text)
        }
    }

    Component {
        id: textProps
        Column {
            spacing: 4
            TextField {
                width: parent ? parent.width : 200
                placeholderText: "Text content"
                text: NitroSourceProps.properties.content || ""
                onEditingFinished: NitroSourceProps.setPropertyValue("content", text)
            }
            TextField {
                width: parent ? parent.width : 200
                placeholderText: "Font"
                text: NitroSourceProps.properties.fontFamily || ""
                onEditingFinished: NitroSourceProps.setPropertyValue("fontFamily", text)
            }
        }
    }

    Component {
        id: colorProps
        TextField {
            width: parent ? parent.width : 200
            placeholderText: "#RRGGBB"
            text: NitroSourceProps.properties.color || ""
            onEditingFinished: NitroSourceProps.setPropertyValue("color", text)
        }
    }

    Component {
        id: audioProps
        Column {
            spacing: 4
            TextField {
                width: parent ? parent.width : 200
                placeholderText: "Device ID (AudioEngine)"
                text: NitroSourceProps.properties.deviceId || ""
                onEditingFinished: NitroSourceProps.setPropertyValue("deviceId", text)
            }
            CheckBox {
                text: "Mute"
                checked: NitroSourceProps.properties.mute || false
                onToggled: NitroSourceProps.setPropertyValue("mute", checked)
            }
        }
    }

    Component {
        id: genericProps
        Item { height: 1 }
    }
}
