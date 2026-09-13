import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

ScrollView {
    id: root

    property int selectedMonitorIndex: -1

    clip: true

    contentWidth: Math.max(1180, availableWidth)
    contentHeight: contentContainer.implicitHeight

    ScrollBar.horizontal.policy: ScrollBar.AsNeeded
    ScrollBar.vertical.policy: ScrollBar.AsNeeded

    Item {
        id: contentContainer

        width: root.contentWidth

        implicitHeight: dashboardLayout.implicitHeight + Theme.spacingXl * 2

        ColumnLayout {
            id: dashboardLayout

            x: Theme.spacingXl
            y: Theme.spacingXl

            width: contentContainer.width - Theme.spacingXl * 2

            spacing: Theme.spacingLg

            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: 180

                clip: true

                ScrollBar.horizontal.policy: ScrollBar.AsNeeded
                ScrollBar.vertical.policy: ScrollBar.AlwaysOff

                contentWidth: monitorRow.implicitWidth
                contentHeight: monitorRow.implicitHeight

                RowLayout {
                    id: monitorRow

                    spacing: Theme.spacingLg

                    Repeater {
                        model: VCPilotAdapter.monitors

                        delegate: MonitorCard {
                            required property var modelData
                            required property int index

                            monitorIndex: index + 1
                            monitorName: modelData.model
                            modelName: modelData.manufacturer
                            internalDisplay: modelData.internalDisplay

                            resolution:
                                modelData.width
                                + "x"
                                + modelData.height

                            primaryMonitor: modelData.primary

                            selected:
                                root.selectedMonitorIndex === index

                            onClicked: {
                                root.selectedMonitorIndex = index
                            }
                        }
                    }

                    ActionCard {
                        dashedBorder: true

                        iconSource: "qrc:/qt/qml/VCPilot/assets/icons/plus.svg"

                        title: "Detect Displays"
                        subtitle: "Scan for new monitors"

                        onClicked: {
                            VCPilotAdapter.refreshMonitors()
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        VCPilotAdapter.refreshMonitors()
    }
}