import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import VCPilot

Rectangle {
    id: root

    required property Window targetWindow

    property var navigationItems: []
    property int currentIndex: 0

    signal navigationRequested(int index)
    signal themeToggleRequested()

    implicitHeight: 72

    color: Theme.background
    border.width: 0

    function toggleMaximized() {
        if (root.targetWindow.visibility === Window.Maximized)
            root.targetWindow.showNormal()
        else
            root.targetWindow.showMaximized()
    }

    RowLayout {
        anchors {
            fill: parent
            leftMargin: Theme.spacingXl
        }

        spacing: Theme.spacingLg

        RowLayout {
            spacing: Theme.spacingMd

            Image {
                source:
                    "qrc:/qt/qml/VCPilot/assets/icons/vcpilot-logo.svg"

                sourceSize.width: 30
                sourceSize.height: 30

                Layout.preferredWidth: 30
                Layout.preferredHeight: 30

                fillMode: Image.PreserveAspectFit
                smooth: true
            }

            Text {
                text: "VCPilot"

                color: Theme.textPrimary
                font.pixelSize: Theme.fontLg
                font.weight: Theme.fontWeightBold
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            MouseArea {
                anchors.fill: parent

                acceptedButtons: Qt.LeftButton

                onPressed: function(mouse) {
                    if (mouse.button === Qt.LeftButton)
                        root.targetWindow.startSystemMove()
                }

                onDoubleClicked: {
                    root.toggleMaximized()
                }
            }
        }

        NavigationBar {
            id: navigationBar

            items: root.navigationItems
            currentIndex: root.currentIndex

            indicatorStyle: NavigationButton.Underline

            onNavigationRequested: function(index) {
                root.navigationRequested(index)
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            MouseArea {
                anchors.fill: parent

                acceptedButtons: Qt.LeftButton

                onPressed: function(mouse) {
                    if (mouse.button === Qt.LeftButton)
                        root.targetWindow.startSystemMove()
                }

                onDoubleClicked: {
                    root.toggleMaximized()
                }
            }
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

        Item {
            Layout.preferredWidth: Theme.spacingSm
        }

        RowLayout {
            spacing: 0

            Layout.alignment: Qt.AlignTop

            TitleBarButton {
                iconSource:
                    "qrc:/qt/qml/VCPilot/assets/icons/window-minimize.svg"

                onClicked: {
                    root.targetWindow.showMinimized()
                }
            }

            TitleBarButton {
                iconSource:
                    root.targetWindow.visibility === Window.Maximized
                        ? "qrc:/qt/qml/VCPilot/assets/icons/window-restore.svg"
                        : "qrc:/qt/qml/VCPilot/assets/icons/window-maximize.svg"

                onClicked: {
                    root.toggleMaximized()
                }
            }

            TitleBarButton {
                iconSource:
                    "qrc:/qt/qml/VCPilot/assets/icons/window-close.svg"

                closeButton: true

                onClicked: {
                    root.targetWindow.close()
                }
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