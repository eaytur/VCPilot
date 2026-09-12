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

        AppIcon {
            source: "qrc:/qt/qml/VCPilot/assets/icons/settings.svg"
            iconSize: 24
            iconColor: Theme.primary
        }
    }
}