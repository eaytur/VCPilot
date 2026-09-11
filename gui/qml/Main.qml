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

        width: 380
        title: "Picture"

        ControlSlider {
            Layout.fillWidth: true

            label: "Brightness"
            value: 72
        }

        ControlSlider {
            Layout.fillWidth: true

            label: "Contrast"
            value: 65
        }

        ControlSlider {
            Layout.fillWidth: true

            label: "Sharpness"
            value: 50
        }
    }
}