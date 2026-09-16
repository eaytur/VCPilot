import QtQuick
import QtQuick.Layouts

import VCPilot

Item {
    id: root

    property string selectedMonitorId: ""

    readonly property var selectedMonitor: {
        for (let i = 0; i < VCPilotAdapter.monitors.length; ++i) {
            const monitor = VCPilotAdapter.monitors[i]

            if (monitor.id === root.selectedMonitorId)
                return monitor
        }

        return null
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingXl

        spacing: Theme.spacingLg

        VcpFeatureTable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 60

            monitorId: root.selectedMonitorId

            features:
                root.selectedMonitor !== null
                ? root.selectedMonitor.vcpFeatures
                : []

            mccsVersion:
                root.selectedMonitor !== null
                ? root.selectedMonitor.mccsVersion
                : ""
        }

        VcpTerminal {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 40

            monitorId: root.selectedMonitorId
        }
    }
}