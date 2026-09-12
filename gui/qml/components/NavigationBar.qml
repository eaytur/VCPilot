import QtQuick
import QtQuick.Layouts

import VCPilot

RowLayout {
    id: root

    property var items: []
    property int currentIndex: 0
    property int indicatorStyle: NavigationButton.Underline

    signal navigationRequested(int index)

    spacing: Theme.spacingSm

    Repeater {
        model: root.items

        delegate: NavigationButton {
            required property int index
            required property var modelData

            text: modelData.text
            iconSource: modelData.icon

            active: root.currentIndex === index
            indicatorStyle: root.indicatorStyle

            onClicked: {
                root.currentIndex = index
                root.navigationRequested(index)
            }
        }
    }
}