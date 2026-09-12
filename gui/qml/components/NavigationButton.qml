import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

Button {
    id: root

    enum IndicatorStyle {
        Outline,
        Underline
    }

    property url iconSource
    property int iconSize: 18
    property bool active: false
    property int indicatorStyle: NavigationButton.Outline

    implicitHeight: 40
    implicitWidth: contentRow.implicitWidth + Theme.spacingLg * 2

    hoverEnabled: true

    background: Item {
        Rectangle {
            anchors.fill: parent

            radius: Theme.radiusSmall
            color: root.hovered
                   ? Theme.surfaceRaised
                   : "transparent"

            border.width: root.active
                          && root.indicatorStyle === NavigationButton.Outline
                          ? 1 : 0

            border.color: Theme.primary
        }

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            height: 2
            radius: 1

            color: Theme.primary

            visible: root.active
                     && root.indicatorStyle === NavigationButton.Underline
        }
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