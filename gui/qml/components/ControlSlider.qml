import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

ColumnLayout {
    id: root

    property string label: ""
    property real value: 0
    property real from: 0
    property real to: 100
    property int decimals: 0

    signal valueEdited(real value)

    spacing: Theme.spacingSm

    RowLayout {
        Layout.fillWidth: true

        Text {
            text: root.label
            color: Theme.textSecondary

            font.pixelSize: Theme.fontMd
            font.weight: Theme.fontWeightMedium
        }

        Item {
            Layout.fillWidth: true
        }

        Text {
            text: Number(root.value).toFixed(root.decimals)
            color: Theme.textPrimary

            font.pixelSize: Theme.fontMd
            font.weight: Theme.fontWeightMedium
        }
    }

    Slider {
        id: slider

        Layout.fillWidth: true

        from: root.from
        to: root.to
        value: root.value

        onMoved: {
            root.value = value
            root.valueEdited(value)
        }

        background: Rectangle {
            x: slider.leftPadding
            y: slider.topPadding + slider.availableHeight / 2 - height / 2

            width: slider.availableWidth
            height: 4

            radius: height / 2
            color: Theme.controlTrack

            Rectangle {
                width: slider.visualPosition * parent.width
                height: parent.height

                radius: parent.radius
                color: Theme.primary
            }
        }

        handle: Rectangle {
            x: slider.leftPadding
               + slider.visualPosition
               * (slider.availableWidth - width)

            y: slider.topPadding
               + slider.availableHeight / 2
               - height / 2

            width: 16
            height: 16

            radius: width / 2

            color: slider.pressed
                ? Theme.controlHandlePressed
                : Theme.controlHandle

            border.width: 2
            border.color: Theme.textPrimary

            Behavior on width {
                NumberAnimation {
                    duration: Theme.animationFast
                }
            }

            Behavior on height {
                NumberAnimation {
                    duration: Theme.animationFast
                }
            }

            states: [
                State {
                    when: slider.hovered && !slider.pressed

                    PropertyChanges {
                        target: slider.handle
                        width: 18
                        height: 18
                    }
                },

                State {
                    when: slider.pressed

                    PropertyChanges {
                        target: slider.handle
                        width: 20
                        height: 20
                    }
                }
            ]
        }
    }
}