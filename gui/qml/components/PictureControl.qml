import QtQuick
import QtQuick.Layouts

import VCPilot

SectionCard {
    id: root

    property string monitorId: ""

    property int brightness: 0
    property int brightnessMaximum: 100

    property int contrast: 0
    property int contrastMaximum: 100

    Layout.fillWidth: true

    ColumnLayout {
        spacing: Theme.spacingMd

        RowLayout {
            spacing: Theme.spacingSm

            AppIcon {
                source:
                    "qrc:/qt/qml/VCPilot/assets/icons/picture.svg"

                iconSize: 18
                iconColor: Theme.primary
            }

            Text {
                text: "Picture"

                color: Theme.textPrimary
                font.pixelSize: Theme.fontMd
                font.weight: Theme.fontWeightBold
            }
        }

        ColumnLayout {
            Layout.fillWidth: true

            spacing: Theme.spacingMd

            ControlSlider {
                Layout.fillWidth: true

                label: "Brightness"

                from: 0
                to:
                    root.brightnessMaximum > 0
                    ? root.brightnessMaximum
                    : 100

                value: root.brightness

                onValueEdited: function(value) {
                    VCPilotAdapter.setBrightness(
                        root.monitorId,
                        Math.round(value)
                    )
                }
            }

            ControlSlider {
                Layout.fillWidth: true

                label: "Contrast"

                from: 0
                to:
                    root.contrastMaximum > 0
                    ? root.contrastMaximum
                    : 100

                value: root.contrast

                onValueEdited: function(value) {
                    VCPilotAdapter.setContrast(
                        root.monitorId,
                        Math.round(value)
                    )
                }
            }
        }
    }
}