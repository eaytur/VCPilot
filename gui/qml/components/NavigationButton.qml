import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

Button {
    id: root

    property url iconSource
    property int iconSize: 18
    property bool active: false

    implicitHeight: 40
    implicitWidth: contentRow.implicitWidth + Theme.spacingLg * 2

    hoverEnabled: true

    background: Rectangle {
        radius: Theme.radiusSmall

        color: {
            if (root.down)
                return Qt.darker(
                    root.active
                        ? Theme.surfaceRaised
                        : Theme.surfaceRaised,
                    1.15
                )

            if (root.active)
                return Theme.surfaceRaised

            if (root.hovered)
                return Theme.surfaceRaised

            return "transparent"
        }

        border.width: root.active ? 1 : 0
        border.color: root.active
                      ? Theme.primary
                      : "transparent"
    }

    contentItem: Item {
        implicitWidth: contentRow.implicitWidth
        implicitHeight: contentRow.implicitHeight

        RowLayout {
            id: contentRow

            anchors.centerIn: parent
            spacing: Theme.spacingSm

            AppIcon {
                source: root.iconSource
                iconSize: root.iconSize

                iconColor: root.active
                           ? Theme.primary
                           : root.hovered
                               ? Theme.textPrimary
                               : Theme.textSecondary
            }

            Text {
                text: root.text

                color: root.active
                       ? Theme.textPrimary
                       : root.hovered
                           ? Theme.textPrimary
                           : Theme.textSecondary

                font.pixelSize: Theme.fontSm
                font.weight: root.active
                             ? Theme.fontWeightBold
                             : Theme.fontWeightMedium
            }
        }
    }
}