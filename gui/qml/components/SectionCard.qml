import QtQuick
import QtQuick.Layouts

import VCPilot

Rectangle {
    id: root

    property string title: ""

    default property alias content: contentLayout.data

    color: Theme.surface
    radius: Theme.radiusLarge

    border.width: 1
    border.color: Theme.border

    implicitWidth: 320
    implicitHeight: contentColumn.implicitHeight
                    + Theme.spacingXl * 2

    ColumnLayout {
        id: contentColumn

        anchors.fill: parent
        anchors.margins: Theme.spacingXl

        spacing: Theme.spacingLg

        Text {
            text: root.title
            color: Theme.textPrimary

            font.pixelSize: Theme.fontLg
            font.weight: Theme.fontWeightBold

            visible: root.title.length > 0
        }

        ColumnLayout {
            id: contentLayout

            Layout.fillWidth: true
            spacing: Theme.spacingMd
        }
    }
}