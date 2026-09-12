import QtQuick
import QtQuick.Controls

import VCPilot

Button {
    id: root

    property url iconSource
    property int iconSize: 20

    implicitWidth: 40
    implicitHeight: 40

    hoverEnabled: true

    background: Rectangle {
        radius: Theme.radiusSmall

        color: root.down
               ? Theme.surfaceRaised
               : root.hovered
                   ? Theme.surfaceRaised
                   : "transparent"

        border.width: 1
        border.color: root.hovered
                      ? Theme.border
                      : "transparent"
    }

    contentItem: AppIcon {
        source: root.iconSource
        iconSize: root.iconSize

        iconColor: root.down
                   ? Theme.primary
                   : root.hovered
                       ? Theme.textPrimary
                       : Theme.textSecondary
    }
}