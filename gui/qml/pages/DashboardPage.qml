import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

ScrollView {
    id: root

    clip: true

    contentWidth: Math.max(1180, availableWidth)
    contentHeight: contentContainer.implicitHeight

    ScrollBar.horizontal.policy: ScrollBar.AsNeeded
    ScrollBar.vertical.policy: ScrollBar.AsNeeded

    Item {
        id: contentContainer

        width: root.contentWidth

        implicitHeight:
            dashboardLayout.implicitHeight
            + Theme.spacingXl * 2

        ColumnLayout {
            id: dashboardLayout

            x: Theme.spacingXl
            y: Theme.spacingXl

            width:
                contentContainer.width
                - Theme.spacingXl * 2

            spacing: Theme.spacingLg
        }
    }
}