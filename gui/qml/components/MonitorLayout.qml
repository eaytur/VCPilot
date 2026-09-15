import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

SectionCard {
    id: root

    property var monitors: []
    property string selectedMonitorId: ""

    signal monitorSelected(string monitorId)

    Layout.fillWidth: true
    Layout.preferredHeight: 500

    readonly property real canvasPadding: Theme.spacingXxl


    readonly property real minX: {
        if (monitors.length === 0)
            return 0

        let value = monitors[0].x

        for (let i = 1; i < monitors.length; ++i)
            value = Math.min(value, monitors[i].x)

        return value
    }

    readonly property real minY: {
        if (monitors.length === 0)
            return 0

        let value = monitors[0].y

        for (let i = 1; i < monitors.length; ++i)
            value = Math.min(value, monitors[i].y)

        return value
    }

    readonly property real maxX: {
        if (monitors.length === 0)
            return 1

        let value = monitors[0].x + monitors[0].width

        for (let i = 1; i < monitors.length; ++i) {
            value = Math.max(
                value,
                monitors[i].x + monitors[i].width
            )
        }

        return value
    }

    readonly property real maxY: {
        if (monitors.length === 0)
            return 1

        let value = monitors[0].y + monitors[0].height

        for (let i = 1; i < monitors.length; ++i) {
            value = Math.max(
                value,
                monitors[i].y + monitors[i].height
            )
        }

        return value
    }

    readonly property real desktopWidth:
        Math.max(1, maxX - minX)

    readonly property real desktopHeight:
        Math.max(1, maxY - minY)

    ColumnLayout {

        spacing: Theme.spacingLg

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingMd

            Rectangle {
                width: 42
                height: 42

                radius: Theme.radiusMedium

                color: Qt.rgba(
                    Theme.primary.r,
                    Theme.primary.g,
                    Theme.primary.b,
                    0.15
                )

                AppIcon {
                    anchors.centerIn: parent

                    source:
                        "qrc:/qt/qml/VCPilot/assets/icons/panels-top-left.svg"

                    iconSize: 22
                    iconColor: Theme.primary
                }
            }

            ColumnLayout {
                spacing: Theme.spacingXs

                Text {
                    text: "Monitor Layout"

                    color: Theme.textPrimary

                    font.pixelSize: Theme.fontLg
                    font.weight: Theme.fontWeightBold
                }

                Text {
                    text:
                        "Displays are positioned according to your layout"

                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSm
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }

        Rectangle {
            id: layoutCanvas

            Layout.fillWidth: true
            Layout.fillHeight: true

            Layout.minimumHeight: 340

            radius: Theme.radiusMedium

            color: Theme.background

            border.width: 1
            border.color: Theme.border

            clip: true

            readonly property real availableWidth:
                Math.max(
                    1,
                    width - root.canvasPadding * 2
                )

            readonly property real availableHeight:
                Math.max(
                    1,
                    height - root.canvasPadding * 2
                )

            readonly property real monitorScale:
                root.monitors.length === 0
                ? 1
                : Math.min(
                    availableWidth / root.desktopWidth,
                    availableHeight / root.desktopHeight
                )

            readonly property real scaledDesktopWidth:
                root.desktopWidth * monitorScale

            readonly property real scaledDesktopHeight:
                root.desktopHeight * monitorScale

            readonly property real offsetX:
                (width - scaledDesktopWidth) / 2

            readonly property real offsetY:
                (height - scaledDesktopHeight) / 2

            Column {
                anchors.centerIn: parent

                visible: root.monitors.length === 0

                spacing: Theme.spacingMd

                AppIcon {
                    anchors.horizontalCenter: parent.horizontalCenter

                    source:
                        "qrc:/qt/qml/VCPilot/assets/icons/monitor.svg"

                    iconSize: 36
                    iconColor: Theme.textMuted
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: "No displays detected"

                    color: Theme.textSecondary

                    font.pixelSize: Theme.fontMd
                    font.weight: Theme.fontWeightMedium
                }
            }

            Repeater {
                model: root.monitors

                delegate: Rectangle {
                    id: monitorItem

                    required property var modelData
                    required property int index

                    readonly property bool selected:
                        root.selectedMonitorId === modelData.id

                    x:
                        layoutCanvas.offsetX
                        + (modelData.x - root.minX)
                        * layoutCanvas.monitorScale

                    y:
                        layoutCanvas.offsetY
                        + (modelData.y - root.minY)
                        * layoutCanvas.monitorScale

                    width:
                        modelData.width
                        * layoutCanvas.monitorScale

                    height:
                        modelData.height
                        * layoutCanvas.monitorScale


                    radius: Theme.radiusSmall

                    color: Theme.surfaceRaised

                    border.width: selected ? 2 : 1

                    border.color:
                        selected
                        ? Theme.primary
                        : Theme.borderStrong

                    Rectangle {
                        anchors {
                            fill: parent
                            margins: 3
                        }

                        radius:
                            Math.max(
                                0,
                                monitorItem.radius - 2
                            )

                        gradient: Gradient {
                            GradientStop {
                                position: 0.0

                                color:
                                    monitorItem.selected
                                    ? Qt.rgba(
                                          Theme.primary.r,
                                          Theme.primary.g,
                                          Theme.primary.b,
                                          0.24
                                      )
                                    : Theme.surfaceRaised
                            }

                            GradientStop {
                                position: 1.0

                                color: Theme.background
                            }
                        }
                    }

                    Rectangle {
                        visible: modelData.primary

                        anchors {
                            top: parent.top
                            right: parent.right

                            topMargin: Theme.spacingSm
                            rightMargin: Theme.spacingSm
                        }

                        implicitWidth:
                            primaryText.implicitWidth
                            + Theme.spacingMd * 2

                        implicitHeight: 28

                        radius: Theme.radiusMedium

                        color: Theme.primary

                        Text {
                            id: primaryText

                            anchors.centerIn: parent

                            text: "Primary"

                            color: Theme.textPrimary

                            font.pixelSize: Theme.fontSm
                            font.weight: Theme.fontWeightMedium
                        }
                    }

                    Column {
                        anchors.centerIn: parent

                        spacing: Theme.spacingXs

                        Text {
                            anchors.horizontalCenter:
                                parent.horizontalCenter

                            text:
                                modelData.model.length > 0
                                ? modelData.model
                                : "Display " + (index + 1)

                            color: Theme.textPrimary

                            font.pixelSize: Theme.fontMd
                            font.weight: Theme.fontWeightBold
                        }

                        Text {
                            anchors.horizontalCenter:
                                parent.horizontalCenter

                            text:
                                modelData.width
                                + " × "
                                + modelData.height

                            color: Theme.textSecondary
                            font.pixelSize: Theme.fontSm
                        }
                    }

                    MouseArea {
                        anchors.fill: parent

                        cursorShape: Qt.PointingHandCursor

                        hoverEnabled: true

                        onClicked: {
                            root.monitorSelected(
                                modelData.id
                            )
                        }
                    }
                }
            }
        }
    }
}