import QtQuick
import QtQuick.Window

import VCPilot

Window {
    id: root

    width: 280
    height: widgetContent.implicitHeight

    visible: false

    flags: Qt.Tool | Qt.FramelessWindowHint

    color: "transparent"
    transientParent: null

    property string widgetName: "Quick Switch"
    property string monitorId: ""
    property string monitorName: ""
    property var sources: []

    signal removeRequested()

    QuickSwitchContent {
        id: widgetContent

        anchors.fill: parent

        widgetName: root.widgetName
        monitorId: root.monitorId
        monitorName: root.monitorName
        sources: root.sources

        currentInputSource: {
            const monitors = VCPilotAdapter.monitors

            for (let i = 0; i < monitors.length; ++i) {
                if (monitors[i].id === root.monitorId)
                    return monitors[i].currentInputSource || ""
            }

            return ""
        }

        onCloseRequested: root.close()
    }

    DragHandler {
        target: null

        onActiveChanged: {
            if (active)
                root.startSystemMove()
        }
    }

    onClosing: function(close) {
        close.accepted = false
        root.hide()
    }
}