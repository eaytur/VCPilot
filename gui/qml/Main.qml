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

        Row {
            spacing: Theme.spacingSm

            NavigationButton {
                text: "Dashboard"
                iconSource: "qrc:/qt/qml/VCPilot/assets/icons/layout-dashboard.svg"
                active: true
            }

            NavigationButton {
                text: "Profiles"
                iconSource: "qrc:/qt/qml/VCPilot/assets/icons/sliders-horizontal.svg"
            }

            NavigationButton {
                text: "Settings"
                iconSource: "qrc:/qt/qml/VCPilot/assets/icons/settings.svg"
            }
        }
    }
}