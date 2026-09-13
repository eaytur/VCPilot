import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

import VCPilot

Rectangle {
    id: root

    property url iconSource
    property string title: ""
    property string subtitle: ""
    property bool dashedBorder: false

    signal clicked()

    implicitWidth: 320
    implicitHeight: 140

    radius: Theme.radiusLarge

    color: mouseArea.containsMouse
           ? Theme.surfaceHover
           : Theme.surface

    border.width: root.dashedBorder ? 0 : 1
    border.color: mouseArea.containsMouse
                  ? Theme.borderStrong
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

    Shape {
        anchors.fill: parent

        visible: root.dashedBorder

        ShapePath {
            strokeWidth: 1

            strokeColor: mouseArea.containsMouse
                         ? Theme.primary
                         : Theme.border

            fillColor: "transparent"

            strokeStyle: ShapePath.DashLine

            dashPattern: [5, 4]

            PathRectangle {
                x: 0.5
                y: 0.5

                width: root.width - 1
                height: root.height - 1

                radius: root.radius
            }
        }
    }

    ColumnLayout {
        anchors.centerIn: parent

        spacing: Theme.spacingSm

        Rectangle {
            Layout.alignment: Qt.AlignHCenter

            implicitWidth: 52
            implicitHeight: 52

            radius: width / 2

            color: Theme.surfaceRaised

            border.width: 1
            border.color: mouseArea.containsMouse
                          ? Theme.primary
                          : Theme.border

            AppIcon {
                anchors.centerIn: parent

                iconSize: 24

                source: root.iconSource

                iconColor: mouseArea.containsMouse
                           ? Theme.primary
                           : Theme.textSecondary
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter

            text: root.title

            color: Theme.textPrimary

            font.pixelSize: Theme.fontMd
            font.weight: Theme.fontWeightBold
        }

        Text {
            Layout.alignment: Qt.AlignHCenter

            text: root.subtitle

            color: Theme.textMuted

            font.pixelSize: Theme.fontSm

            visible: root.subtitle.length > 0
        }
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent

        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        onClicked: root.clicked()
    }
}