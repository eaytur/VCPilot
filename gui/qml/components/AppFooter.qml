import QtQuick
import QtQuick.Layouts

import VCPilot

Rectangle {
    id: root

    property string selectedMonitorId: ""

    implicitHeight: 68

    color: Theme.surface
    border.width: 1
    border.color: Theme.border

    property var selectedMonitor: {
        for (let i = 0; i < VCPilotAdapter.monitors.length; ++i) {
            if (VCPilotAdapter.monitors[i].id === selectedMonitorId)
                return VCPilotAdapter.monitors[i]
        }

        return null
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Theme.spacingXl
        anchors.rightMargin: Theme.spacingXl

        spacing: Theme.spacingSm

        StatusIndicator {
            status: root.selectedMonitor
                    ? StatusIndicator.Info
                    : StatusIndicator.Neutral
        }

        Text {
            text: root.selectedMonitor
                  ? "Selected:"
                  : "No monitor selected"

            color: Theme.textSecondary
            font.pixelSize: Theme.fontSm
        }

        Text {
            visible: root.selectedMonitor !== null

            text: root.selectedMonitor
                  ? root.selectedMonitor.model
                  : ""

            color: Theme.textPrimary
            font.pixelSize: Theme.fontSm
            font.weight: Theme.fontWeightBold
        }

        Item {
            Layout.fillWidth: true
        }
    }
}