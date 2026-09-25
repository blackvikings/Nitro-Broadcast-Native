import QtQuick
import QtQuick.Controls
import NitroBroadcast

NitroPanel {
    id: root
    title: "SCENES"

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
                label: "Add scene"
                onClicked: NitroScenes.createScene("")
            }
            NitroIconButton {
                text: "⧉"
                label: "Duplicate"
                onClicked: NitroScenes.duplicateScene(NitroScenes.previewIndex)
            }
            NitroIconButton {
                text: "✎"
                label: "Rename"
                onClicked: renameDialog.open()
            }
            NitroIconButton {
                text: "−"
                label: "Delete"
                onClicked: NitroScenes.deleteScene(NitroScenes.previewIndex)
            }
        }

        ListView {
            id: list
            anchors.fill: parent
            anchors.topMargin: 36
            anchors.margins: 6
            clip: true
            spacing: 4
            model: NitroScenes

            delegate: NitroSceneItem {
                width: list.width
                name: model.name
                isPreview: model.isPreview
                isProgram: model.isProgram
                onClicked: NitroScenes.selectPreview(index)
                onDuplicateRequested: NitroScenes.duplicateScene(index)
                onDeleteRequested: NitroScenes.deleteScene(index)
                onRenameRequested: {
                    renameDialog.targetIndex = index
                    renameDialog.renameText = model.name
                    renameDialog.open()
                }
            }
        }
    }

    Dialog {
        id: renameDialog
        property int targetIndex: 0
        property string renameText: ""
        title: "Rename Scene"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: Overlay.overlay
        onAccepted: NitroScenes.renameScene(targetIndex, renameField.text)

        TextField {
            id: renameField
            width: 260
            text: renameDialog.renameText
            color: Theme.textPrimary
            background: Rectangle {
                color: Theme.bgInput
                border.color: Theme.borderSubtle
                radius: 4
            }
        }
    }
}
