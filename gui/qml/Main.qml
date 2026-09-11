import QtQuick
import QtQuick.Controls

import VCPilot

ApplicationWindow {
    width: 1200
    height: 760

    visible: true
    title: "VCPilot"

    color: Theme.background

    SectionCard {
        anchors.centerIn: parent

        width: 380
        title: "Picture"

        Text {
            text: "Brightness"
            color: Theme.textSecondary
            font.pixelSize: Theme.fontMd
        }

        Text {
            text: "Contrast"
            color: Theme.textSecondary
            font.pixelSize: Theme.fontMd
        }

        Text {
            text: "Sharpness"
            color: Theme.textSecondary
            font.pixelSize: Theme.fontMd
        }
    }
}