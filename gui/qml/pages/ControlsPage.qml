import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

ScrollView {
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

    readonly property bool monitorControllable:
        selectedMonitor !== null
        && selectedMonitor.controllable

    onSelectedMonitorIdChanged: {
        VCPilotAdapter.selectMonitor(root.selectedMonitorId)
    }

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

            RowLayout {
                id: controlsContainer

                visible:
                    root.selectedMonitorId !== ""
                    && root.monitorControllable

                Layout.fillWidth: true
                Layout.minimumHeight: 540

                spacing: Theme.spacingLg

                CalibrationControl {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredWidth: 48
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredWidth: 52

                    spacing: Theme.spacingLg

                    PictureControl {
                        Layout.fillWidth: true

                        monitorId:
                            root.selectedMonitorId

                        brightness:
                            root.selectedMonitor !== null
                            ? root.selectedMonitor.brightness
                            : 0

                        brightnessMaximum:
                            root.selectedMonitor !== null
                            ? root.selectedMonitor.brightnessMaximum
                            : 100

                        contrast:
                            root.selectedMonitor !== null
                            ? root.selectedMonitor.contrast
                            : 0

                        contrastMaximum:
                            root.selectedMonitor !== null
                            ? root.selectedMonitor.contrastMaximum
                            : 100
                    }

                    InputSourceControl {
                        Layout.fillWidth: true

                        monitorId:
                            root.selectedMonitorId

                        currentInputSource:
                            root.selectedMonitor !== null
                            ? root.selectedMonitor.currentInputSource
                            : ""
                    }
                    AudioControl {
                        Layout.fillWidth: true

                        monitorId:
                            root.selectedMonitorId

                        volume:
                            root.selectedMonitor !== null
                            ? root.selectedMonitor.volume
                            : 0

                        volumeMaximum:
                            root.selectedMonitor !== null
                            ? root.selectedMonitor.volumeMaximum
                            : 100

                        muted:
                            root.selectedMonitor !== null
                            ? root.selectedMonitor.muted
                            : false
                    }
                    Item {
                        Layout.fillHeight: true
                    }
                }
            }
            
            Item {
                visible:
                    root.selectedMonitorId !== ""
                    && root.selectedMonitor !== null
                    && !root.monitorControllable

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 360

                ColumnLayout {
                    anchors.centerIn: parent

                    spacing: Theme.spacingMd

                    AppIcon {
                        Layout.alignment:
                            Qt.AlignHCenter

                        source:
                            "qrc:/qt/qml/VCPilot/assets/icons/circle-alert.svg"

                        iconSize: 44
                    }

                    Label {
                        Layout.alignment:
                            Qt.AlignHCenter

                        text:
                            "Display controls unavailable"

                        color:
                            Theme.textPrimary

                        font.pixelSize:
                            Theme.fontLg

                        font.weight:
                            Theme.fontWeightMedium
                    }

                    Label {
                        Layout.alignment:
                            Qt.AlignHCenter

                        Layout.maximumWidth: 420

                        text:
                            root.selectedMonitor
                            ? (
                                root.selectedMonitor.internalDisplay
                                ? "Built-in displays are not currently supported by VCPilot."
                                : "This display cannot currently be controlled by VCPilot."
                            )
                            : ""

                        horizontalAlignment:
                            Text.AlignHCenter

                        wrapMode:
                            Text.WordWrap

                        color:
                            Theme.textSecondary

                        font.pixelSize:
                            Theme.fontMd
                    }
                }
            }

            Label {
                visible:
                    root.selectedMonitorId === ""

                Layout.fillWidth: true

                text:
                    "Select a display to view its controls."

                horizontalAlignment:
                    Text.AlignHCenter

                color:
                    Theme.textSecondary

                font.pixelSize:
                    Theme.fontMd
            }
        }
    }
}