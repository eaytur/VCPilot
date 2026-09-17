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

    function valuesString(feature) {
        if (feature.values && feature.values.length > 0) {
            let result = []

            for (let i = 0; i < feature.values.length; ++i)
                result.push(root.codeString(feature.values[i]))

            return result.join(", ")
        }

        if (feature.type === "Continuous")
            return "Continuous"

        return "—"
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

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
                visible: root.features
                         && root.features.length > 0

                text:
                    root.features.length
                    + (root.features.length === 1
                       ? " feature"
                       : " features")

                color: Theme.textSecondary
                font.pixelSize: Theme.fontSm
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
                    Layout.preferredWidth: 70

                    text: "Code"

                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSm
                    font.weight: Theme.fontWeightMedium
                }

                Text {
                    Layout.preferredWidth: 210

                    text: "Feature"

                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSm
                    font.weight: Theme.fontWeightMedium
                }

                Text {
                    Layout.preferredWidth: 100

                    text: "Access"

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
            id: featureList

            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true
            spacing: 2

            model: root.features

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            delegate: Rectangle {
                required property var modelData

                width: ListView.view.width
                height: 44

                radius: 6
                color: mouseArea.containsMouse
                       ? Theme.controlTrack
                       : "transparent"

                RowLayout {
                    anchors.fill: parent

                    anchors.leftMargin: Theme.spacingMd
                    anchors.rightMargin: Theme.spacingMd

                    spacing: Theme.spacingMd

                    Text {
                        Layout.preferredWidth: 70

                        text:
                            root.codeString(
                                modelData.code)

                        color: Theme.primary

                        font.pixelSize: Theme.fontSm
                        font.family: "monospace"
                    }

                    Text {
                        Layout.preferredWidth: 210

                        text:
                            modelData.name
                            ? modelData.name
                            : "Unknown VCP Feature"

                        color: Theme.textPrimary

                        font.pixelSize: Theme.fontSm

                        elide: Text.ElideRight
                    }

                    Text {
                        Layout.preferredWidth: 100

                        text:
                            modelData.access
                            ? modelData.access
                            : "Unknown"

                        color: Theme.textSecondary

                        font.pixelSize: Theme.fontSm

                        elide: Text.ElideRight
                    }

                    Text {
                        Layout.fillWidth: true

                        text:
                            root.valuesString(
                                modelData)

                        color: Theme.textSecondary

                        font.pixelSize: Theme.fontSm

                        elide: Text.ElideRight
                    }
                }

                MouseArea {
                    id: mouseArea

                    anchors.fill: parent

                    hoverEnabled: true
                    acceptedButtons: Qt.NoButton
                }
            }
        }

        Item {
            visible:
                !root.features
                || root.features.length === 0

            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                anchors.centerIn: parent

                text:
                    root.monitorId === ""
                    ? "Select a display"
                    : "No VCP capabilities reported"

                color: Theme.textSecondary
                font.pixelSize: Theme.fontSm
            }
        }
    }
}