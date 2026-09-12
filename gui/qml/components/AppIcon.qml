import QtQuick
import QtQuick.Effects

Item {
    id: root

    property url source
    property int iconSize: 20
    property color iconColor: Theme.textSecondary

    width: iconSize
    height: iconSize

    Image {
        id: iconImage

        anchors.fill: parent
        source: root.source

        sourceSize.width: root.iconSize
        sourceSize.height: root.iconSize

        fillMode: Image.PreserveAspectFit
        smooth: true
        visible: false
    }

    MultiEffect {
        anchors.fill: parent
        source: iconImage

        colorization: 1.0
        colorizationColor: root.iconColor
    }
}