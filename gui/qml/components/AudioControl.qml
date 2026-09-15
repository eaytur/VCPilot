import QtQuick
import QtQuick.Layouts

import VCPilot

SectionCard {
    id: root

    property string monitorId: ""

    property int volume: 0
    property int volumeMaximum: 100
    property bool muted: false

    Layout.fillWidth: true

    ColumnLayout {
        Layout.fillWidth: true

        spacing: Theme.spacingMd

        RowLayout {
            Layout.fillWidth: true

            spacing: Theme.spacingSm

            AppIcon {
                source: "qrc:/qt/qml/VCPilot/assets/icons/volume-2.svg"

                iconSize: 18
                iconColor: Theme.primary
            }

            Text {
                text: "Audio"

                color: Theme.textPrimary
                font.pixelSize: Theme.fontMd
                font.weight: Theme.fontWeightBold
            }
        }

        RowLayout {
            Layout.fillWidth: true

            spacing: Theme.spacingSm

            IconButton {
                id: muteButton

                iconSize: 20

                iconSource:
                    root.muted || root.volume === 0
                    ? "qrc:/qt/qml/VCPilot/assets/icons/volume-off.svg"
                    : "qrc:/qt/qml/VCPilot/assets/icons/volume-2.svg"

                onClicked: {
                    if (root.monitorId.length === 0)
                        return

                    VCPilotAdapter.setMute(
                        root.monitorId,
                        !root.muted
                    )
                }
            }

            ControlSlider {
                Layout.fillWidth: true

                label: "Volume"

                from: 0

                to:
                    root.volumeMaximum > 0
                    ? root.volumeMaximum
                    : 100

                value: root.volume

                onValueEdited: function(value) {
                    if (root.monitorId.length === 0)
                        return

                    const newVolume = Math.round(value)

                    VCPilotAdapter.setVolume(
                        root.monitorId,
                        newVolume
                    )

                    if (root.muted && newVolume > 0) {
                        VCPilotAdapter.setMute(
                            root.monitorId,
                            false
                        )
                    }
                }
            }
        }
    }
}