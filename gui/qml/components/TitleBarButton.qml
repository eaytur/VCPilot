import QtQuick
import QtQuick.Controls

AbstractButton {
    id: root

    property url iconSource
    property bool closeButton: false

    implicitWidth: 46
    implicitHeight: 36

    hoverEnabled: true

    background: Rectangle {
        color:
            root.hovered
                ? root.closeButton
                    ? "#C42B1C"
                    : Theme.surfaceRaised
                : "transparent"
    }

    contentItem: Item {
        AppIcon {
            anchors.centerIn: parent

            source: root.iconSource
            iconSize: 12

            iconColor:
                root.hovered
                    ? Theme.textPrimary
                    : Theme.textSecondary
        }
    }
}
