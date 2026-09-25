import QtQuick
import NitroBroadcast

Item {
    id: root
    property string name: "display"
    property color iconColor: Theme.textSecondary
    width: 16
    height: 16

    // Path-based vector glyphs — no emoji dependency
    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.clearRect(0, 0, width, height)
            ctx.strokeStyle = root.iconColor
            ctx.fillStyle = root.iconColor
            ctx.lineWidth = 1.5
            ctx.lineCap = "round"
            ctx.lineJoin = "round"
            var s = Math.min(width, height)
            ctx.translate((width - s) / 2, (height - s) / 2)
            ctx.scale(s / 16, s / 16)

            function rect(x, y, w, h) { ctx.strokeRect(x, y, w, h) }
            function circle(x, y, r) {
                ctx.beginPath(); ctx.arc(x, y, r, 0, Math.PI * 2); ctx.stroke()
            }

            switch (root.name) {
            case "display":
                rect(2, 3, 12, 8); ctx.beginPath(); ctx.moveTo(6, 13); ctx.lineTo(10, 13); ctx.stroke()
                break
            case "window":
                rect(2, 2, 12, 12); ctx.beginPath(); ctx.moveTo(2, 5); ctx.lineTo(14, 5); ctx.stroke()
                break
            case "camera":
                rect(1, 4, 10, 8); ctx.beginPath(); ctx.moveTo(11, 6); ctx.lineTo(15, 4); ctx.lineTo(15, 12); ctx.lineTo(11, 10); ctx.closePath(); ctx.stroke()
                break
            case "mic":
                rect(6, 2, 4, 7); ctx.beginPath(); ctx.moveTo(4, 8); ctx.quadraticCurveTo(4, 12, 8, 12); ctx.quadraticCurveTo(12, 12, 12, 8); ctx.stroke()
                ctx.beginPath(); ctx.moveTo(8, 12); ctx.lineTo(8, 14); ctx.moveTo(6, 14); ctx.lineTo(10, 14); ctx.stroke()
                break
            case "speaker":
                ctx.beginPath(); ctx.moveTo(2, 6); ctx.lineTo(5, 6); ctx.lineTo(9, 3); ctx.lineTo(9, 13); ctx.lineTo(5, 10); ctx.lineTo(2, 10); ctx.closePath(); ctx.stroke()
                circle(12, 8, 2)
                break
            case "image":
                rect(2, 3, 12, 10); ctx.beginPath(); ctx.moveTo(2, 11); ctx.lineTo(6, 7); ctx.lineTo(9, 10); ctx.lineTo(11, 8); ctx.lineTo(14, 11); ctx.stroke()
                circle(11, 6, 1.2)
                break
            case "text":
                ctx.beginPath(); ctx.moveTo(3, 4); ctx.lineTo(13, 4); ctx.moveTo(8, 4); ctx.lineTo(8, 13); ctx.stroke()
                break
            case "color":
                rect(3, 3, 10, 10); ctx.fillRect(5, 5, 6, 6)
                break
            case "browser":
                circle(8, 8, 6); ctx.beginPath(); ctx.moveTo(2, 8); ctx.lineTo(14, 8); ctx.moveTo(8, 2); ctx.lineTo(8, 14); ctx.stroke()
                break
            case "scene":
                rect(2, 3, 8, 6); rect(6, 7, 8, 6)
                break
            case "group":
                rect(2, 2, 5, 5); rect(9, 2, 5, 5); rect(5, 9, 5, 5)
                break
            case "media":
                rect(2, 3, 12, 10); ctx.beginPath(); ctx.moveTo(7, 6); ctx.lineTo(11, 8); ctx.lineTo(7, 10); ctx.closePath(); ctx.fill()
                break
            default:
                // generic diamond
                ctx.beginPath(); ctx.moveTo(8, 2); ctx.lineTo(14, 8); ctx.lineTo(8, 14); ctx.lineTo(2, 8); ctx.closePath(); ctx.stroke()
                break
            }
        }
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
    }

    onNameChanged: canvas.requestPaint()
    onIconColorChanged: canvas.requestPaint()
    Component.onCompleted: canvas.requestPaint()
}
