import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

ColumnLayout {
    id: root

    property var options: []
    property int currentIndex: 0

    signal selectionChanged(int index)

    Flow {
        Layout.fillWidth: true
        spacing: Theme.spacingSm

        Repeater {
            model: root.options.length

            delegate: Button {
                required property int index
                readonly property int optionIndex: index

                implicitHeight: 40
                implicitWidth: contentItem.implicitWidth + Theme.spacingXl

                text: root.options[optionIndex]

                background: Rectangle {
                    radius: Theme.radiusSmall
                    color: Theme.surfaceRaised
                    border.width: 1
                    border.color: optionIndex === root.currentIndex
                                  ? Theme.primary : Theme.border
                }

                contentItem: Text {
                    text: parent.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: optionIndex === root.currentIndex
                           ? Theme.textPrimary : Theme.textSecondary
                    font.pixelSize: Theme.fontSm
                    font.weight: Theme.fontWeightMedium
                }

                onClicked: {
                    root.currentIndex = optionIndex
                    root.selectionChanged(optionIndex)
                }
            }
        }
    }
}