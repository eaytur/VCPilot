import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

Button {
    id: root

    enum IconPosition {
        Left,
        Right
    }

    property url iconSource
    property int iconSize: 18
    property bool primary: false
    property int iconPosition: AppButton.Left

    implicitHeight: 40
    implicitWidth: contentRow.implicitWidth + Theme.spacingLg * 2

    hoverEnabled: true

    background: Rectangle {
        radius: Theme.radiusSmall

        color:
            root.primary
                ? root.down
                    ? Theme.primaryPressed
                    : root.hovered
                        ? Theme.primaryHover
                        : Theme.primary
                : root.down
                    ? Qt.darker(Theme.surface, 1.25)
                    : root.hovered
                        ? Theme.surfaceRaised
                        : Theme.surface

        border.width: 1

        border.color:
            root.primary
                ? color
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

            LayoutMirroring.enabled: root.iconPosition === AppButton.Right

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