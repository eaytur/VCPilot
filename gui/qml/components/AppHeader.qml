import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

Rectangle {
    id: root

    property var navigationItems: []
    property int currentIndex: 0

    signal navigationRequested(int index)
    signal themeToggleRequested()

    implicitHeight: 72

    color: Theme.background

    border.width: 0

    RowLayout {
        anchors {
            fill: parent
            leftMargin: Theme.spacingXl
            rightMargin: Theme.spacingXl
        }

        spacing: Theme.spacingLg

        RowLayout {
            spacing: Theme.spacingMd

            AppIcon {
                source: "qrc:/qt/qml/VCPilot/assets/icons/monitor.svg"
                iconSize: 30
                iconColor: Theme.primary
            }

            ColumnLayout {
                spacing: 0

                Text {
                    text: "VCPilot"

                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontLg
                    font.weight: Theme.fontWeightBold
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }

        NavigationBar {
            id: navigationBar

            items: root.navigationItems
            currentIndex: root.currentIndex

            indicatorStyle: NavigationButton.Underline

            onNavigationRequested: function(index) {
                root.currentIndex = index
                root.navigationRequested(index)
            }
        }

        Item {
            Layout.fillWidth: true
        }

        AppSwitch {
            checked: Theme.darkMode

            leftIcon:
                "qrc:/qt/qml/VCPilot/assets/icons/sun.svg"

            rightIcon:
                "qrc:/qt/qml/VCPilot/assets/icons/moon.svg"

            onToggled: function(checked) {
                Theme.darkMode = checked
            }
        }
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: 1
        color: Theme.border
    }
}