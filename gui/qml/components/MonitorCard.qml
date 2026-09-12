import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

Rectangle {
    id: root

    property string monitorName: ""
    property string modelName: ""
    property string connection: ""
    property string resolution: ""
    property bool primaryMonitor: false
    property bool selected: false
    property int monitorIndex: 0

    signal clicked()

    implicitWidth: 280
    implicitHeight: 140

    radius: Theme.radiusLarge
    color: selected
           ? Theme.surfaceRaised
           : Theme.surface

    border.width: 1
    border.color: selected
                  ? Theme.primary
                  : Theme.border

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor

        onClicked: root.clicked()
    }

    ColumnLayout {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right

            topMargin: Theme.spacingLg
            leftMargin: Theme.spacingLg
            rightMargin: Theme.spacingLg
        }

        spacing: Theme.spacingSm

        RowLayout {
            Layout.fillWidth: true

            Text {
                Layout.fillWidth: true

                text: root.monitorName
                color: Theme.textPrimary

                font.pixelSize: Theme.fontLg
                font.weight: Theme.fontWeightBold

                elide: Text.ElideRight
            }

            Rectangle {
                implicitWidth: 28
                implicitHeight: 28

                radius: Theme.radiusSmall

                color: root.selected
                       ? Theme.primary
                       : Theme.surfaceRaised

                border.width: 1
                border.color: root.selected
                              ? Theme.primary
                              : Theme.border

                Text {
                    anchors.centerIn: parent

                    text: root.monitorIndex + 1

                    color: root.selected
                           ? Theme.textPrimary
                           : Theme.textSecondary

                    font.pixelSize: Theme.fontSm
                    font.weight: Theme.fontWeightMedium
                }
            }
        }

        Text {
            text: root.modelName
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSm
        }

        Text {
            text: root.resolution + " · " + root.connection
            color: Theme.textMuted
            font.pixelSize: Theme.fontSm
        }
    }

    Rectangle {
        visible: root.primaryMonitor

        anchors {
            right: parent.right
            bottom: parent.bottom

            rightMargin: Theme.spacingLg
            bottomMargin: Theme.spacingLg
        }

        implicitWidth: primaryText.implicitWidth
                       + Theme.spacingMd * 2
        implicitHeight: 26

        radius: Theme.radiusSmall
        color: Theme.surfaceRaised

        border.width: 1
        border.color: Theme.primary

        Text {
            id: primaryText
            anchors.centerIn: parent

            text: "Primary"
            color: Theme.primary

            font.pixelSize: Theme.fontXs
            font.weight: Theme.fontWeightMedium
        }
    }
}