import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 1200
    height: 760

    visible: true

    title: "VCPilot"

    Rectangle {
        anchors.fill: parent
        color: "#111318"

        Text {
            anchors.centerIn: parent

            text: "VCPilot"
            color: "white"

            font.pixelSize: 42
            font.bold: true
        }
    }
}