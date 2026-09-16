import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

SectionCard {
    id: root

    property string monitorId: ""
    property var features: []
    property string mccsVersion: ""

    Layout.fillWidth: true
    Layout.fillHeight: true

    function codeString(code) {
        return "0x"
            + Number(code)
                .toString(16)
                .toUpperCase()
                .padStart(2, "0")
    }

    function featureName(code) {
        switch (code) {
        case 0x10:
            return "Brightness"

        case 0x12:
            return "Contrast"

        case 0x14:
            return "Color Preset"

        case 0x16:
            return "Red Gain"

        case 0x18:
            return "Green Gain"

        case 0x1A:
            return "Blue Gain"

        case 0x60:
            return "Input Source"

        case 0x62:
            return "Audio Volume"

        case 0x8D:
            return "Audio Mute"

        case 0xD6:
            return "Power Mode"

        default:
            return "VCP Feature"
        }
    }

    function valuesString(values) {
        if (!values || values.length === 0)
            return "—"

        let result = []

        for (let i = 0; i < values.length; ++i)
            result.push(codeString(values[i]))

        return result.join(", ")
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        
        spacing: Theme.spacingMd

        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "VCP Features"

                color: Theme.textPrimary
                font.pixelSize: Theme.fontMd
                font.weight: Theme.fontWeightBold
            }

            Item {
                Layout.fillWidth: true
            }

            Text {
                visible: root.mccsVersion !== ""

                text: "MCCS " + root.mccsVersion

                color: Theme.textSecondary
                font.pixelSize: Theme.fontSm
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 36

            radius: 6
            color: Theme.controlTrack

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingMd
                anchors.rightMargin: Theme.spacingMd

                spacing: Theme.spacingMd

                Text {
                    Layout.preferredWidth: 90

                    text: "Code"

                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSm
                    font.weight: Theme.fontWeightMedium
                }

                Text {
                    Layout.preferredWidth: 180

                    text: "Feature"

                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSm
                    font.weight: Theme.fontWeightMedium
                }

                Text {
                    Layout.fillWidth: true

                    text: "Supported Values"

                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSm
                    font.weight: Theme.fontWeightMedium
                }
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true

            model: root.features

            spacing: 2

            delegate: Rectangle {
                required property var modelData

                width: ListView.view.width
                height: 42

                radius: 6
                color: "transparent"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: Theme.spacingMd
                    anchors.rightMargin: Theme.spacingMd

                    spacing: Theme.spacingMd

                    Text {
                        Layout.preferredWidth: 90

                        text:
                            root.codeString(
                                modelData.code)

                        color: Theme.primary
                        font.pixelSize: Theme.fontSm
                        font.family: "monospace"
                    }

                    Text {
                        Layout.preferredWidth: 180

                        text:
                            root.featureName(
                                modelData.code)

                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontSm
                    }

                    Text {
                        Layout.fillWidth: true

                        text:
                            root.valuesString(
                                modelData.values)

                        color: Theme.textSecondary
                        font.pixelSize: Theme.fontSm
                        font.family: "monospace"

                        elide: Text.ElideRight
                    }
                }
            }
        }

        Text {
            visible:
                !root.features
                || root.features.length === 0

            Layout.alignment: Qt.AlignHCenter

            text:
                root.monitorId === ""
                ? "Select a display"
                : "No VCP capabilities reported"

            color: Theme.textSecondary
            font.pixelSize: Theme.fontSm
        }
    }
}