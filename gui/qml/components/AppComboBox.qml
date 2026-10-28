import QtQuick
import QtQuick.Controls

import VCPilot

ComboBox {
    id: root

    implicitWidth: 180
    implicitHeight: 40

    hoverEnabled: true

    leftPadding: Theme.spacingMd
    rightPadding: 38

    font.pixelSize: Theme.fontSm

    contentItem: Text {
        text: root.displayText

        color: root.enabled
               ? Theme.textPrimary
               : Theme.textMuted

        font.pixelSize: Theme.fontSm

        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        radius: Theme.radiusSmall

        color: root.pressed
               ? Theme.surfaceHover
               : root.hovered
                 ? Theme.surfaceHover
                 : Theme.surfaceRaised

        border.width: 1

        border.color: root.activeFocus
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

            color: root.pressed || root.hovered
                   ? Theme.primary
                   : Theme.textSecondary

            font.pixelSize: Theme.fontLg

            rotation: root.popup.visible ? 180 : 0

            Behavior on color {
                ColorAnimation {
                    duration: Theme.animationFast
                }
            }

            Behavior on rotation {
                NumberAnimation {
                    duration: Theme.animationFast
                }
            }
        }
    }

    delegate: ItemDelegate {
        id: delegateItem

        required property int index

        width: root.width - root.popup.leftPadding
                          - root.popup.rightPadding
        height: 38

        hoverEnabled: true

        leftPadding: Theme.spacingMd
        rightPadding: Theme.spacingMd

        highlighted: root.highlightedIndex === index

        contentItem: Text {
            text: root.textAt(delegateItem.index)

            color: delegateItem.enabled
                   ? Theme.textPrimary
                   : Theme.textMuted

            font.pixelSize: Theme.fontSm

            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        background: Rectangle {
            radius: Theme.radiusSmall

            color: delegateItem.hovered
                   || delegateItem.highlighted
                   ? Theme.surfaceHover
                   : "transparent"

            Behavior on color {
                ColorAnimation {
                    duration: Theme.animationFast
                }
            }
        }
    }

    popup: Popup {
        y: root.height + Theme.spacingXs

        width: root.width

        padding: Theme.spacingXs

        implicitHeight: Math.min(
            contentItem.implicitHeight + topPadding + bottomPadding,
            280
        )

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

            ScrollIndicator.vertical: ScrollIndicator {}
        }
    }
}