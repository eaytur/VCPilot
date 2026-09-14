import QtQuick

import VCPilot

Item {
    id: root

    property bool running: false

    property color trackColor: Theme.surfaceRaised
    property color barColor: Theme.primary

    implicitWidth: 180
    implicitHeight: 5

    opacity: running ? 1.0 : 0.0
    visible: opacity > 0

    Rectangle {
        id: track

        anchors.fill: parent

        radius: height / 2
        color: root.trackColor
        clip: true

        Rectangle {
            id: shimmer

            width: Math.max(40, track.width * 0.35)
            height: track.height

            radius: height / 2

            gradient: Gradient {
                orientation: Gradient.Horizontal

                GradientStop {
                    position: 0.0
                    color: Qt.rgba(
                        root.barColor.r,
                        root.barColor.g,
                        root.barColor.b,
                        0.0
                    )
                }

                GradientStop {
                    position: 0.5
                    color: root.barColor
                }

                GradientStop {
                    position: 1.0
                    color: Qt.rgba(
                        root.barColor.r,
                        root.barColor.g,
                        root.barColor.b,
                        0.0
                    )
                }
            }

            x: -width

            SequentialAnimation on x {
                running: root.running
                loops: Animation.Infinite

                NumberAnimation {
                    from: -shimmer.width
                    to: track.width
                    duration: 1000
                    easing.type: Easing.InOutQuad
                }

                PauseAnimation {
                    duration: 100
                }
            }
        }
    }

    Behavior on opacity {
        NumberAnimation {
            duration: Theme.animationNormal
        }
    }
}