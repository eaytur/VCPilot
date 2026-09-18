import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Rectangle {
    id: root

    required property Window targetWindow

    color: Theme.background

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            id: dragArea

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
                    if (root.targetWindow.visibility === Window.Maximized)
                        root.targetWindow.showNormal()
                    else
                        root.targetWindow.showMaximized()
                }
            }
        }

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
                if (root.targetWindow.visibility === Window.Maximized)
                    root.targetWindow.showNormal()
                else
                    root.targetWindow.showMaximized()
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