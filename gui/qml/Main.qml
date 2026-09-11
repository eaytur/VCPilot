import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

ApplicationWindow {
    width: 1200
    height: 760

    visible: true
    title: "VCPilot"

    color: Theme.background

    SectionCard {
        anchors.centerIn: parent

        width: 420
        title: "Input Source"

        SegmentedControl {
            Layout.fillWidth: true

            options: [
                "DisplayPort 1",
                "HDMI 1",
                "HDMI 1",
                "HDMI 1",
                "HDMI 1"
            ]
        }
    }
}