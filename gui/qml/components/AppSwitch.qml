import QtQuick

import VCPilot

Item {
    id: root

    property bool checked: false

    property url leftIcon
    property url rightIcon

    property color activeColor: Theme.primary
    property color inactiveColor: Theme.textMuted

    signal toggled(bool checked)

    implicitWidth: 68
    implicitHeight: 34

    Rectangle {
        anchors.fill: parent

        radius: height / 2
        color: Theme.surface
        border.width: 1
        border.color: Theme.border
    }

    // Sliding active indicator
    Rectangle {
        id: indicator

        width: 28
        height: 28

        y: 3

        x: root.checked
           ? root.width - width - 3
           : 3

        radius: width / 2
        color: Theme.surfaceRaised

        border.width: 1
        border.color: Theme.primary

        Behavior on x {
            NumberAnimation {
                duration: Theme.animationFast
            }
        }
    }

    AppIcon {
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
            leftMargin: 9
        }

        iconSize: 16
        source: root.leftIcon

        iconColor: !root.checked
                   ? root.activeColor
                   : root.inactiveColor
    }

    AppIcon {
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            rightMargin: 9
        }

        iconSize: 16
        source: root.rightIcon

        iconColor: root.checked
                   ? root.activeColor
                   : root.inactiveColor
    }

    MouseArea {
        anchors.fill: parent

        cursorShape: Qt.PointingHandCursor

        onClicked: {
            root.checked = !root.checked
            root.toggled(root.checked)
        }
    }
}