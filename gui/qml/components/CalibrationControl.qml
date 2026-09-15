import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

SectionCard {
    id: root

    property string selectedPattern: "Off"

    Layout.fillWidth: true
    Layout.fillHeight: true

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        spacing: Theme.spacingMd

        RowLayout {
            Layout.fillWidth: true

            spacing: Theme.spacingSm

            AppIcon {
                source:
                    "qrc:/qt/qml/VCPilot/assets/icons/calibration.svg"

                iconSize: 18
                iconColor: Theme.primary
            }

            Text {
                text: "Calibration"

                color: Theme.textPrimary
                font.pixelSize: Theme.fontMd
                font.weight: Theme.fontWeightBold
            }

            Item {
                Layout.fillWidth: true
            }

            AppComboBox {
                id: patternComboBox

                Layout.preferredWidth: 180

                model: [
                    "Off",
                    "Contrast"
                ]

                onCurrentTextChanged: {
                    root.selectedPattern = currentText
                }
            }
        }

        Rectangle {
            id: preview

            Layout.fillWidth: true
            Layout.fillHeight: true

            Layout.minimumHeight: 420

            radius: Theme.radiusMedium
            clip: true

            color: Theme.controlTrack

            Image {
                anchors.fill: parent

                visible:
                    root.selectedPattern !== "Off"

                source: "qrc:/qt/qml/VCPilot/assets/images/contrast-calib.png"

                fillMode: Image.Stretch
                smooth: true
            }

            ColumnLayout {
                visible:
                    root.selectedPattern === "Off"

                anchors.centerIn: parent

                spacing: Theme.spacingSm

                AppIcon {
                    Layout.alignment:
                        Qt.AlignHCenter

                    source:
                        "qrc:/qt/qml/VCPilot/assets/icons/image.svg"

                    iconSize: 36
                    iconColor: Theme.textSecondary
                }

                Text {
                    Layout.alignment:
                        Qt.AlignHCenter

                    text:
                        "Select a calibration pattern"

                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontMd
                }
            }
        }
    }
}