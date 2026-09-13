import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

ScrollView {
    id: root

    property string selectedMonitorId: ""

    signal controlRequested()
    signal monitorSelected(string monitorId)

    clip: true

    contentWidth: Math.max(1180, availableWidth)
    contentHeight: contentContainer.implicitHeight

    ScrollBar.horizontal.policy: ScrollBar.AsNeeded
    ScrollBar.vertical.policy: ScrollBar.AsNeeded

    Item {
        id: contentContainer

        width: root.contentWidth

        implicitHeight:
            dashboardLayout.implicitHeight
            + Theme.spacingXl * 2

        ColumnLayout {
            id: dashboardLayout

            x: Theme.spacingXl
            y: Theme.spacingXl

            width:
                contentContainer.width
                - Theme.spacingXl * 2

            spacing: Theme.spacingLg

            SectionCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 88

                RowLayout {
                    x: Theme.spacingXl
                    y: (parent.height - implicitHeight) / 2

                    width:
                        parent.width
                        - Theme.spacingXl * 2

                    spacing: Theme.spacingSm

                    StatusIndicator {
                        id: monitorStatus

                        status:
                            VCPilotAdapter.monitors.length > 0
                            ? StatusIndicator.Success
                            : StatusIndicator.Warning
                    }

                    Text {
                        text:
                            VCPilotAdapter.monitors.length === 0
                            ? "No monitors found"
                            : VCPilotAdapter.monitors.length === 1
                                ? "1 monitor found"
                                : VCPilotAdapter.monitors.length
                                  + " monitors found"

                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontSm
                        font.weight: Theme.fontWeightMedium
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    AppButton {
                        primary: true

                        text: "Control monitors"

                        iconSource:
                            "qrc:/qt/qml/VCPilot/assets/icons/forward.svg"

                        iconPosition: AppButton.Right

                        onClicked: root.controlRequested()
                    }
                }
            }

            MonitorLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 500

                monitors: VCPilotAdapter.monitors
                selectedMonitorId: root.selectedMonitorId

                onMonitorSelected: function(monitorId) {
                    root.monitorSelected(monitorId)
                }
            }
        }
    }
}