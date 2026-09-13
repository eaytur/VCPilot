import QtQuick

import VCPilot

Item {
    id: root

    enum Status {
        Neutral,
        Success,
        Warning,
        Error,
        Info
    }

    property int status: StatusIndicator.Neutral 
    
    readonly property color indicatorColor: {
        switch (status) {
        case StatusIndicator.Neutral:
            return Theme.neutral
        case StatusIndicator.Success:
            return Theme.success
        case StatusIndicator.Warning:
            return Theme.warning
        case StatusIndicator.Error:
            return Theme.error
        case StatusIndicator.Info:
            return Theme.info
        default:
            return Theme.textFaint
        }
    }

    implicitWidth: 18
    implicitHeight: 18

    Rectangle {
        anchors.centerIn: parent

        width: 10
        height: 10

        radius: width / 2
        color: root.indicatorColor

        Rectangle {
            anchors.centerIn: parent

            width: 18
            height: 18

            radius: width / 2

            color: "transparent"

            border.width: 4
            border.color: Qt.rgba(
                root.indicatorColor.r,
                root.indicatorColor.g,
                root.indicatorColor.b,
                0.15
            )
        }
    }
}