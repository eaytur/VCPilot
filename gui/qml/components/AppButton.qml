import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

Button {
    id: root

    property url iconSource
    property int iconSize: 18
    property bool primary: false

    implicitHeight: 40
    implicitWidth: contentRow.implicitWidth + Theme.spacingLg * 2

    hoverEnabled: true

    background: Rectangle {
        radius: Theme.radiusSmall

        readonly property color baseColor:
            root.primary
                ? Theme.primary
                : root.hovered
                    ? Theme.surfaceRaised
                    : Theme.surface

        color: root.down
            ? Qt.darker(baseColor, 1.25)
            : baseColor

        border.width: 1
        border.color: root.primary
                    ? Theme.primary
                    : root.hovered
                        ? Theme.textMuted
                        : Theme.border
    }

    contentItem: Item {
        implicitWidth: contentRow.implicitWidth
        implicitHeight: contentRow.implicitHeight

        RowLayout {
            id: contentRow

            anchors.centerIn: parent
            spacing: Theme.spacingSm

            AppIcon {
                visible: root.iconSource.toString().length > 0

                source: root.iconSource
                iconSize: root.iconSize

                iconColor: root.primary
                        ? Theme.textPrimary
                        : root.hovered
                            ? Theme.textPrimary
                            : Theme.textSecondary
            }

            Text {
                text: root.text

                color: root.primary
                    ? Theme.textPrimary
                    : root.hovered
                        ? Theme.textPrimary
                        : Theme.textSecondary

                font.pixelSize: Theme.fontSm
                font.weight: Theme.fontWeightMedium
            }
        }
    }
}