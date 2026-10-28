
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

Rectangle {
    id: root

    implicitWidth: 280
    implicitHeight: content.implicitHeight + 24

    radius: Theme.radiusLarge
    color: Theme.surface

    border.width: 1
    border.color: Theme.border

    property string widgetName: "Quick Switch"
    property string monitorId: ""
    property string monitorName: ""
    property var sources: []

    property bool previewMode: false
    property string currentInputSource: ""

    signal closeRequested()

    ColumnLayout {
        id: content

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 12

        spacing: 12

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 9

            Rectangle {
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30

                radius: Theme.radiusSmall
                color: Theme.surfaceRaised

                AppIcon {
                    anchors.centerIn: parent
                    source: "qrc:/qt/qml/VCPilot/assets/icons/arrow-left-right.svg"
                    iconSize: 17
                    iconColor: Theme.primary
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Text {
                    text: root.widgetName
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSm
                    font.bold: true
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    text: root.monitorName
                    color: Theme.textSecondary
                    font.pixelSize: 11
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }

            Rectangle {
                id: closeButton

                visible: !root.previewMode

                Layout.preferredWidth: 24
                Layout.preferredHeight: 24

                radius: Theme.radiusSmall

                color: closeMouse.containsMouse
                       ? Theme.surfaceHover
                       : "transparent"

                Behavior on color {
                    ColorAnimation {
                        duration: Theme.animationFast
                    }
                }

                AppIcon {
                    anchors.centerIn: parent
                    source: "qrc:/qt/qml/VCPilot/assets/icons/window-close.svg"
                    iconSize: 14
                    iconColor: closeMouse.containsMouse
                               ? "#EF4444"
                               : Theme.textSecondary
                }

                MouseArea {
                    id: closeMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.closeRequested()
                }
            }
        }

        // Input sources
        GridLayout {
            Layout.fillWidth: true

            columns: 2
            columnSpacing: 8
            rowSpacing: 8

            Repeater {
                model: root.sources

                delegate: Rectangle {
                    id: inputButton

                    required property var modelData

                    Layout.fillWidth: true
                    Layout.preferredHeight: 34

                    radius: Theme.radiusSmall

                    readonly property bool active:
                        root.currentInputSource === modelData.key

                    color: active
                           ? Theme.primary
                           : mouseArea.containsMouse && !root.previewMode
                             ? Theme.surfaceHover
                             : Theme.surfaceRaised

                    border.width: 1
                    border.color: active
                                  ? Theme.primary
                                  : Theme.border

                    Behavior on color {
                        ColorAnimation {
                            duration: Theme.animationFast
                        }
                    }

                    Text {
                        anchors.fill: parent
                        anchors.leftMargin: 6
                        anchors.rightMargin: 6

                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter

                        text: inputButton.modelData.label

                        color: inputButton.active
                               ? "#FFFFFF"
                               : Theme.textPrimary

                        font.pixelSize: 12
                        font.bold: inputButton.active

                        elide: Text.ElideRight
                    }

                    MouseArea {
                        id: mouseArea

                        anchors.fill: parent
                        hoverEnabled: true
                        enabled: !root.previewMode

                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            VCPilotAdapter.setInputSource(
                                root.monitorId,
                                inputButton.modelData.key
                            )
                        }
                    }
                }
            }
        }
    }
}
