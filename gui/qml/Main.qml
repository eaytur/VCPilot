import QtQuick
import QtQuick.Controls

import VCPilot

ApplicationWindow {
    width: 1200
    height: 760

    visible: true
    title: "VCPilot"

    color: Theme.background

    Rectangle {
        anchors.centerIn: parent

        width: 320
        height: 160

        radius: Theme.radiusLarge
        color: Theme.surface
        border.color: Theme.border
        border.width: 1

        Column {
            anchors.centerIn: parent
            spacing: Theme.spacingSm

            Text {
                anchors.horizontalCenter: parent.horizontalCenter

                text: "VCPilot"
                color: Theme.textPrimary

                font.pixelSize: Theme.fontDisplay
                font.weight: Theme.fontWeightBold
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter

                text: "Control your displays."
                color: Theme.textSecondary

                font.pixelSize: Theme.fontSm
            }
        }
    }
}