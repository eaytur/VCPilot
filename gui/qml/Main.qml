import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

ApplicationWindow {
    id: window

    width: 1200
    height: 760
    visible: true
    title: "VCPilot"

    color: Theme.background

    property int selectedMonitor: 0

    RowLayout {
        anchors.centerIn: parent
        spacing: Theme.spacingLg

        MonitorCard {
            monitorName: "Samsung Odyssey G3"
            modelName: "LS27DG30X"
            resolution: "1920 × 1080"
            connection: "DisplayPort"

            monitorIndex: 0
            primaryMonitor: true

            selected: window.selectedMonitor === 0

            onClicked: {
                window.selectedMonitor = 0
            }
        }

        MonitorCard {
            monitorName: "Samsung Odyssey G3"
            modelName: "LS24AG32X"
            resolution: "1920 × 1080"
            connection: "HDMI"

            monitorIndex: 1
            primaryMonitor: false

            selected: window.selectedMonitor === 1

            onClicked: {
                window.selectedMonitor = 1
            }
        }
    }
}