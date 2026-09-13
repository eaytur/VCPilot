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
    property bool internalDisplay: false
    property int monitorIndex: 0
    property url iconSource: root.internalDisplay
        ? "qrc:/qt/qml/VCPilot/assets/icons/laptop.svg"
        : "qrc:/qt/qml/VCPilot/assets/icons/monitor.svg"

    signal clicked()

    implicitWidth: 320
    implicitHeight: 140

    radius: Theme.radiusLarge
    color: root.selected
           ? Theme.surfaceRaised
           : Theme.surface

    border.width: 1
    border.color: root.selected
                  ? Theme.primary
                  : Theme.border

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor

        onClicked: root.clicked()
    }

    RowLayout {
        anchors {
            fill: parent
            margins: Theme.spacingLg
        }

        spacing: Theme.spacingLg

        Rectangle {
            Layout.preferredWidth: 72
            Layout.preferredHeight: 72
            Layout.alignment: Qt.AlignVCenter

            radius: Theme.radiusMedium

            color: root.selected
                   ? Qt.rgba(
                         Theme.primary.r,
                         Theme.primary.g,
                         Theme.primary.b,
                         0.10
                     )
                   : Theme.surfaceRaised

            border.width: 1

            border.color: root.selected
                          ? Theme.primary
                          : Theme.border

            AppIcon {
                anchors.centerIn: parent

                iconSize: 34

                source: root.iconSource 

                iconColor: root.selected
                           ? Theme.primary
                           : Theme.textSecondary
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter

            spacing: 3

            RowLayout {
                Layout.fillWidth: true

                spacing: Theme.spacingSm

                Text {
                    Layout.fillWidth: true

                    text: root.monitorName
                    color: Theme.textPrimary

                    font.pixelSize: Theme.fontMd
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

                        text: root.monitorIndex

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
                font.pixelSize: Theme.fontXs
            }

            Text {
                text: root.resolution 
                color: Theme.textSecondary
                font.pixelSize: Theme.fontXs

                elide: Text.ElideRight
            }

            Text {
                text:root.connection
                color: Theme.textSecondary
                font.pixelSize: Theme.fontXs
                font.weight: Theme.fontWeightBold
                
                elide: Text.ElideRight
            }

            Item {
                visible: root.primaryMonitor

                implicitHeight: 24
            }
        }
    }

    Rectangle {
        visible: primaryMonitor

        anchors {
            right: parent.right
            bottom: parent.bottom

            rightMargin: Theme.spacingLg
            bottomMargin: Theme.spacingLg
        }

        implicitWidth: primaryText.implicitWidth
                       + Theme.spacingMd * 2
        implicitHeight: 24

        radius: Theme.radiusSmall
        color: "transparent"

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