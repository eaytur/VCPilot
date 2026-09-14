import QtQuick
import QtQuick.Layouts

import VCPilot

SectionCard {
    id: root

    property string monitorId: ""

    Layout.fillWidth: true

    ColumnLayout {
        spacing: Theme.spacingMd

        RowLayout {
            spacing: Theme.spacingSm

            AppIcon {
                source:
                    "qrc:/qt/qml/VCPilot/assets/icons/controls.svg"

                iconSize: 18
                iconColor: Theme.primary
            }

            Text {
                text: "Input Source"

                color: Theme.textPrimary
                font.pixelSize: Theme.fontMd
                font.weight: Theme.fontWeightBold
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingSm

            Repeater {
                model: VCPilotAdapter.inputSources

                delegate: AppButton {
                    required property var modelData

                    Layout.fillWidth: true
                    text: modelData.name

                    onClicked: {
                        VCPilotAdapter.setInputSource(
                            root.monitorId,
                            modelData.key
                        )
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        if (root.monitorId.length > 0) {
            VCPilotAdapter.loadInputControl(
                root.monitorId
            )
        }
    }

    onMonitorIdChanged: {
        if (root.monitorId.length > 0) {
            VCPilotAdapter.loadInputControl(
                root.monitorId
            )
        }
    }
}