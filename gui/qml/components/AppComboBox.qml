import QtQuick
import QtQuick.Controls

import VCPilot

ComboBox {
    id: root

    implicitWidth: 180
    implicitHeight: 40

    leftPadding: Theme.spacingMd
    rightPadding: 38

    font.pixelSize: Theme.fontSm

    contentItem: Text {
        text: root.displayText

        color:
            root.enabled
            ? Theme.textPrimary
            : Theme.textMuted

        font.pixelSize: Theme.fontSm

        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        radius: Theme.radiusSmall

        color:
            root.pressed
            ? Theme.surfaceHover
            : Theme.surfaceRaised

        border.width: root.activeFocus ? 1 : 1

        border.color:
            root.activeFocus
            ? Theme.primary
            : Theme.border

        Behavior on color {
            ColorAnimation {
                duration: Theme.animationFast
            }
        }

        Behavior on border.color {
            ColorAnimation {
                duration: Theme.animationFast
            }
        }
    }

    indicator: Item {
        width: 32
        height: root.height

        x: root.width - width
        y: 0

        Text {
            anchors.centerIn: parent

            text: "⌄"

            color:
                root.pressed
                ? Theme.primary
                : Theme.textSecondary

            font.pixelSize: Theme.fontLg

            rotation:
                root.popup.visible
                ? 180
                : 0

            Behavior on rotation {
                NumberAnimation {
                    duration: Theme.animationFast
                }
            }
        }
    }

    delegate: ItemDelegate {
        required property var modelData

        width: root.width
        height: 38

        leftPadding: Theme.spacingMd
        rightPadding: Theme.spacingMd

        contentItem: Text {
            text: modelData

            color: Theme.textPrimary
            font.pixelSize: Theme.fontSm

            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            radius: Theme.radiusSmall

            color:
                parent.highlighted
                ? Theme.surfaceHover
                : "transparent"
        }
    }

    popup: Popup {
        y: root.height + Theme.spacingXs

        width: root.width

        implicitHeight:
            contentItem.implicitHeight
            + Theme.spacingSm

        padding: Theme.spacingXs

        background: Rectangle {
            color: Theme.surfaceRaised

            radius: Theme.radiusSmall

            border.width: 1
            border.color: Theme.border
        }

        contentItem: ListView {
            clip: true

            implicitHeight: contentHeight

            model: root.popup.visible
                   ? root.delegateModel
                   : null

            currentIndex: root.highlightedIndex

            spacing: 2

            ScrollIndicator.vertical:
                ScrollIndicator {}
        }
    }
}