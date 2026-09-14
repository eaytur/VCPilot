import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

ScrollView {
    id: root

    property var monitors
    property string selectedMonitorId: ""

    signal monitorSelected(string monitorId)
    signal refreshRequested()

    Layout.preferredHeight: 180

    clip: true

    ScrollBar.horizontal.policy: ScrollBar.AsNeeded
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    contentWidth: monitorRow.implicitWidth
    contentHeight: monitorRow.implicitHeight

    RowLayout {
        id: monitorRow

        x: Theme.spacingXl
        y: Theme.spacingXl

        spacing: Theme.spacingLg

        Repeater {
            model: root.monitors

            delegate: MonitorCard {
                required property var modelData
                required property int index

                monitorIndex: index + 1

                monitorName: modelData.model
                modelName: modelData.manufacturer

                internalDisplay: modelData.internalDisplay
                inputSource: modelData.currentInputSource
                
                resolution:
                    modelData.width
                    + "x"
                    + modelData.height

                primaryMonitor: modelData.primary

                selected:
                    root.selectedMonitorId === modelData.id

                onClicked: {
                    root.monitorSelected(modelData.id)
                }
            }
        }

        ActionCard {
            id: detectCard

            dashedBorder: true

            enabled: !VCPilotAdapter.detecting

            iconSource:
                "qrc:/qt/qml/VCPilot/assets/icons/plus.svg"

            title:
                VCPilotAdapter.detecting
                    ? "Detecting Displays"
                    : "Detect Displays"

            subtitle:
                VCPilotAdapter.detecting
                    ? "Scanning connected monitors..."
                    : "Scan for new monitors"

            ScanProgressBar {
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom

                    leftMargin: Theme.spacingLg
                    rightMargin: Theme.spacingLg
                    bottomMargin: Theme.spacingSm
                }

                height: 5

                running: VCPilotAdapter.detecting

                z: 10
            }

            onClicked: {
                root.refreshRequested()
            }
        }
    }
}