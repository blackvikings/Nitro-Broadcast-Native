pragma Singleton
import QtQuick

QtObject {
    id: theme

    readonly property bool isDark: true

    readonly property color bgApp: "#0E0F12"
    readonly property color bgPanel: "#16181D"
    readonly property color bgElevated: "#1C1F26"
    readonly property color bgInput: "#12141A"
    readonly property color bgHover: "#22262F"
    readonly property color borderSubtle: "#2A2E37"
    readonly property color textPrimary: "#E8EAED"
    readonly property color textSecondary: "#9AA0A6"
    readonly property color textDisabled: "#5F6368"
    readonly property color accent: "#5B8CFF"
    readonly property color accentPressed: "#4A78E0"
    readonly property color preview: "#2DD4BF"
    readonly property color program: "#F07178"
    readonly property color success: "#3DDC97"
    readonly property color warning: "#F5A524"
    readonly property color danger: "#E85D75"
    readonly property color meterOk: "#3DDC97"
    readonly property color meterWarn: "#F5A524"
    readonly property color meterClip: "#E85D75"

    readonly property int radiusPanel: 6
    readonly property int radiusControl: 4
    readonly property int radiusChip: 2

    readonly property int spaceXs: 4
    readonly property int spaceSm: 8
    readonly property int spaceMd: 12
    readonly property int spaceLg: 16
    readonly property int spaceXl: 24

    readonly property int fontXs: 11
    readonly property int fontSm: 12
    readonly property int fontMd: 13
    readonly property int fontLg: 14
    readonly property int fontXl: 16
    readonly property int fontTitle: 18

    readonly property font uiFont: Qt.font({ family: "Segoe UI", pixelSize: fontMd })
    readonly property font monoFont: Qt.font({ family: "Consolas", pixelSize: fontSm })
}
