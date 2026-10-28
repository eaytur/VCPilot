import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import VCPilot
Item {
    id: root
    property string selectedMonitorId: ""
    property string configuredMonitorId: ""
    property string configuredMonitorName: ""
    property var configuredSources: []
    property var createdWidgets: []
    property int activeTab: 0
    property bool configuring: false
    property bool restoringWidgets: false
    property var validatedMonitorIds: ({})

    Connections {
        target: VCPilotAdapter

        function onMonitorsChanged() {
            root.validateWidgetMonitors()
        }
    }

    function loadMonitor(monitorId) {
        configuredMonitorId = monitorId
        configuredMonitorName = ""
        configuredSources = []
        const monitors = VCPilotAdapter.monitors
        for (let i = 0; i < monitors.length; ++i) {
            if (monitors[i].id === monitorId) {
                configuredMonitorName =
                    monitors[i].model || monitors[i].id
                break
            }
        }
        if (!monitorId)
            return
        VCPilotAdapter.loadInputSources(monitorId)
        const detected = VCPilotAdapter.inputSources
        const sources = []
        for (let i = 0; i < detected.length; ++i) {
            sources.push({
                key: detected[i].key,
                name: detected[i].name,
                label: detected[i].name
            })
        }
        configuredSources = sources
    }
    function updateSourceLabel(index, label) {
        const updated = configuredSources.slice()
        updated[index] = {
            key: updated[index].key,
            name: updated[index].name,
            label: label
        }
        configuredSources = updated
    }
    function createWidget() {
        if (!configuredMonitorId || configuredSources.length === 0)
            return

        const sources = configuredSources.map(function(source) {
            return {
                key: source.key,
                label: source.label.trim() || source.name
            }
        })

        const offset = createdWidgets.length * 30

        const config = {
            id: "widget-" + Date.now()
                + "-" + Math.floor(Math.random() * 1000000),
            type: "quickSwitch",
            name: widgetNameField.text.trim() || "Quick Switch",
            monitorId: configuredMonitorId,
            monitorName: configuredMonitorName,
            sources: sources,
            x: 100 + offset,
            y: 100 + offset,
            visible: true
        }

        if (!addWidget(config))
            return

        saveWidgets()

        configuring = false
        activeTab = 1
    }

    function removeWidget(widget) {
        const exists = createdWidgets.some(function(entry) {
            return entry.window === widget
        })

        if (!exists)
            return

        createdWidgets = createdWidgets.filter(function(entry) {
            return entry.window !== widget
        })

        widget.hide()
        const entry = createdWidgets.find(function(e) {
            return e.window === widget
        })

        if (entry)
            delete validatedMonitorIds[entry.id]       
        widget.destroy()

        saveWidgets()
    }

    function saveWidgets() {
        if (restoringWidgets)
            return

        const data = createdWidgets.map(function(entry) {
            const widget = entry.window

            return {
                id: entry.id,
                type: "quickSwitch",
                name: widget.widgetName,
                monitorId: widget.monitorId,
                monitorName: widget.monitorName,
                sources: widget.sources,
                x: widget.x,
                y: widget.y,
                visible: widget.visible
            }
        })

        if (!VCPilotAdapter.saveWidgets(data))
            console.error("Failed to persist widgets")
    }

    function addWidget(config) {
        const widget = quickSwitchComponent.createObject(null, {
            widgetName: config.name,
            monitorId: config.monitorId,
            monitorName: config.monitorName,
            sources: config.sources
        })

        if (!widget)
            return null

        const entry = {
            id: config.id,
            name: config.name,
            monitorName: config.monitorName,
            window: widget
        }

        widget.x = config.x
        widget.y = config.y

        widget.removeRequested.connect(function() {
            widget.hide()
            root.saveWidgets()
        })

        widget.xChanged.connect(function() {
            saveTimer.restart()
        })

        widget.yChanged.connect(function() {
            saveTimer.restart()
        })

        widget.visibleChanged.connect(function() {
            saveTimer.restart()
        })

        createdWidgets = createdWidgets.concat([entry])

        if (config.visible)
            widget.show()

        return widget
    }

    function restoreWidgets() {
        restoringWidgets = true

        const saved = VCPilotAdapter.loadWidgets()

        for (let i = 0; i < saved.length; ++i) {
            const config = saved[i]

            if (config.type !== "quickSwitch")
                continue

            addWidget(config)
        }

        restoringWidgets = false
        if (VCPilotAdapter.monitors.length > 0)
            validateWidgetMonitors()
    }

    function shutdownWidgets() {
        saveTimer.stop()

        root.saveWidgets()

        const widgets = createdWidgets
        createdWidgets = []

        for (let i = 0; i < widgets.length; ++i) {
            const widget = widgets[i].window

            if (widget) {
                widget.hide()
                widget.destroy()
            }
        }
    }

    function validateWidgetMonitors() {
        const monitors = VCPilotAdapter.monitors

        if (monitors.length === 0)
            return

        for (const entry of createdWidgets) {
            const widget = entry.window

            const monitor = monitors.find(function(m) {
                return m.id === widget.monitorId && m.controllable
            })

            const connected = monitor !== undefined
            const previous = validatedMonitorIds[entry.id]

            if (previous === connected)
                continue

            validatedMonitorIds[entry.id] = connected

            if (connected) {
                console.log(
                    "[Widget] Monitor connected:",
                    widget.widgetName,
                    "->",
                    monitor.model
                )
            } else {
                console.warn(
                    "[Widget] Monitor unavailable:",
                    widget.widgetName
                )
            }
        }
    }

    Timer {
        id: saveTimer

        interval: 400
        repeat: false

        onTriggered: root.saveWidgets()
    }

    Component.onCompleted: restoreWidgets()

    Component {
        id: quickSwitchComponent
        QuickSwitchWidget {
            transientParent: null
        }
    }
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingMd
        spacing: Theme.spacingMd
        // Page header
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 6
            Text {
                text: "Desktop Widgets"
                color: Theme.textPrimary
                font.pixelSize: 24
                font.bold: true
            }
            Text {
                text: "Create and manage your desktop widgets."
                color: Theme.textSecondary
                font.pixelSize: Theme.fontSm
            }
        }
        // Navigation
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingSm
            AppButton {
                text: "Widget Store"
                onClicked: root.activeTab = 0
            }
            AppButton {
                text: "My Widgets"
                onClicked: root.activeTab = 1
            }
            Item {
                Layout.fillWidth: true
            }
            Text {
                text: root.createdWidgets.length + " widgets"
                color: Theme.textSecondary
                font.pixelSize: Theme.fontSm
            }
        }
        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: root.activeTab
            // =========================
            // WIDGET STORE
            // =========================
            ScrollView {
                id: storeScroll
                clip: true
                ColumnLayout {
                    width: Math.max(0, storeScroll.availableWidth)
                    spacing: Theme.spacingMd
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight:
                            storeContent.implicitHeight
                            + Theme.spacingMd * 2
                        radius: Theme.radiusLarge
                        color: Theme.surface
                        border.width: 1
                        border.color: Theme.border
                        ColumnLayout {
                            id: storeContent
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.margins: Theme.spacingMd
                            spacing: Theme.spacingMd
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: Theme.spacingMd

                                Rectangle {
                                    Layout.preferredWidth: 48
                                    Layout.preferredHeight: 48
                                    radius: Theme.radiusMedium
                                    color: Theme.surfaceRaised

                                    AppIcon {
                                        anchors.centerIn: parent
                                        source: "qrc:/qt/qml/VCPilot/assets/icons/arrow-left-right.svg"
                                        iconSize: 22
                                        iconColor: Theme.primary
                                    }
                                }

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 5

                                    Text {
                                        text: "Quick Switch"
                                        Layout.fillWidth: true
                                        color: Theme.textPrimary
                                        font.pixelSize: Theme.fontMd
                                        font.bold: true
                                    }

                                    Text {
                                        text: "Switch between your monitor's input sources."
                                        Layout.fillWidth: true
                                        color: Theme.textSecondary
                                        font.pixelSize: Theme.fontSm
                                        wrapMode: Text.WordWrap
                                    }
                                }

                                Rectangle {
                                    id: configureButton
                                    Layout.preferredWidth: 126
                                    Layout.preferredHeight: 40
                                    Layout.alignment: Qt.AlignVCenter
                                    radius: Theme.radiusSmall
                                    color: configureMouse.containsMouse
                                           ? Theme.surfaceHover : Theme.surfaceRaised
                                    border.width: 1
                                    border.color: configureMouse.containsMouse
                                                  ? Theme.primary : Theme.border

                                    Behavior on color {
                                        ColorAnimation { duration: Theme.animationFast }
                                    }

                                    RowLayout {
                                        anchors.centerIn: parent
                                        spacing: Theme.spacingSm

                                        AppIcon {
                                            source: root.configuring
                                                    ? "qrc:/qt/qml/VCPilot/assets/icons/window-close.svg"
                                                    : "qrc:/qt/qml/VCPilot/assets/icons/settings-2.svg"
                                            iconSize: 17
                                            iconColor: configureMouse.containsMouse
                                                       ? Theme.primary : Theme.textSecondary
                                        }

                                        Text {
                                            text: root.configuring ? "Cancel" : "Configure"
                                            color: configureMouse.containsMouse
                                                   ? Theme.textPrimary : Theme.textSecondary
                                            font.pixelSize: Theme.fontSm
                                            font.bold: true
                                        }
                                    }

                                    MouseArea {
                                        id: configureMouse
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        cursorShape: Qt.PointingHandCursor

                                        onClicked: {
                                            root.configuring = !root.configuring
                                            if (root.configuring)
                                                root.loadMonitor(monitorCombo.currentValue || "")
                                        }
                                    }
                                }
                            }
                            // Configuration
                            ColumnLayout {
                                visible: root.configuring
                                Layout.fillWidth: true
                                spacing: Theme.spacingMd
                                Rectangle {
                                    Layout.fillWidth: true
                                    height: 1
                                    color: Theme.border
                                }
                                Text {
                                    text: "Widget Name"
                                    color: Theme.textPrimary
                                    font.pixelSize: Theme.fontSm
                                }
                                AppTextField {
                                    id: widgetNameField
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 40
                                    text: "Quick Switch"
                                    placeholderText: "Widget name"
                                }
                                Text {
                                    text: "Target Monitor"
                                    color: Theme.textPrimary
                                    font.pixelSize: Theme.fontSm
                                }
                                AppComboBox {
                                    id: monitorCombo
                                    Layout.fillWidth: true
                                    model: VCPilotAdapter.monitors.filter(
                                        function(monitor) {
                                            return monitor.controllable
                                        }
                                    )
                                    textRole: "model"
                                    valueRole: "id"
                                    Component.onCompleted: {
                                        const index = indexOfValue(
                                            root.selectedMonitorId
                                        )
                                        if (index >= 0)
                                            currentIndex = index
                                    }
                                    onCurrentValueChanged: {
                                        if (root.configuring) {
                                            root.loadMonitor(
                                                currentValue || ""
                                            )
                                        }
                                    }
                                }
                                Text {
                                    text: "Detected Input Sources"
                                    color: Theme.textPrimary
                                    font.pixelSize: Theme.fontMd
                                    font.bold: true
                                }
                                Text {
                                    text: "Customize the names displayed on your widget."
                                    color: Theme.textSecondary
                                    font.pixelSize: Theme.fontSm
                                }
                                Repeater {
                                    model: root.configuredSources
                                    delegate: RowLayout {
                                        required property int index
                                        required property var modelData
                                        Layout.fillWidth: true
                                        spacing: Theme.spacingMd
                                        Text {
                                            text: modelData.name
                                            color: Theme.textSecondary
                                            font.pixelSize: Theme.fontSm
                                            Layout.preferredWidth: 150
                                        }
                                        AppTextField {
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: 40
                                            text: modelData.label
                                            placeholderText: modelData.name
                                            onEditingFinished: {
                                                root.updateSourceLabel(index, text)
                                            }
                                        }
                                    }
                                }
                                Text {
                                    visible: root.configuredSources.length === 0
                                    text: "No supported input sources detected."
                                    color: Theme.textSecondary
                                    font.pixelSize: Theme.fontSm
                                }
                                RowLayout {
                                    Layout.fillWidth: true
                                    Item {
                                        Layout.fillWidth: true
                                    }
                                    AppButton {
                                        text: "Add Widget"
                                        enabled:
                                            root.configuredMonitorId !== ""
                                            && root.configuredSources.length > 0
                                        onClicked: root.createWidget()
                                    }
                                }
                            }
                        }
                    }
                }
            }
            // =========================
            // MY WIDGETS
            // =========================
            Item {
                id: myWidgetsPage

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: Theme.spacingMd
                    visible: root.createdWidgets.length === 0

                    AppIcon {
                        source: "qrc:/qt/qml/VCPilot/assets/icons/layout-grid.svg"
                        iconSize: 40
                        iconColor: Theme.textMuted
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Text {
                        text: "No widgets yet"
                        Layout.alignment: Qt.AlignHCenter
                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontMd
                        font.bold: true
                    }

                    Text {
                        text: "Create your first widget from the Widget Store."
                        Layout.alignment: Qt.AlignHCenter
                        color: Theme.textSecondary
                        font.pixelSize: Theme.fontSm
                    }

                    AppButton {
                        text: "Browse Widgets"
                        Layout.alignment: Qt.AlignHCenter
                        onClicked: root.activeTab = 0
                    }
                }

                ScrollView {
                    id: widgetsScroll
                    anchors.fill: parent
                    clip: true
                    visible: root.createdWidgets.length > 0

                    ColumnLayout {
                        width: Math.max(0, widgetsScroll.availableWidth)
                        spacing: Theme.spacingMd
                    // Widget gallery
                    GridLayout {
                        Layout.fillWidth: true
                        columns: width >= 720 ? 2 : 1
                        columnSpacing: Theme.spacingMd
                        rowSpacing: Theme.spacingMd
                        Repeater {
                            model: root.createdWidgets
                            delegate: Rectangle {
                                id: widgetCard
                                required property var modelData
                                Layout.fillWidth: true
                                Layout.preferredHeight:
                                    cardContent.implicitHeight
                                    + Theme.spacingMd * 2
                                radius: Theme.radiusLarge
                                color: Theme.surface
                                border.width: 1
                                border.color: Theme.border
                                ColumnLayout {
                                    id: cardContent
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.top: parent.top
                                    anchors.margins: Theme.spacingMd
                                    spacing: Theme.spacingMd
                                    // Card header
                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: Theme.spacingSm
                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 4
                                            Text {
                                                text: widgetCard.modelData.name
                                                color: Theme.textPrimary
                                                font.pixelSize: Theme.fontMd
                                                font.bold: true
                                                elide: Text.ElideRight
                                                Layout.fillWidth: true
                                            }
                                            Text {
                                                text: "Quick Switch"
                                                color: Theme.textSecondary
                                                font.pixelSize: Theme.fontSm
                                            }
                                        }
                                        // Show / Hide
                                        Rectangle {
                                            width: 36
                                            height: 36
                                            radius: Theme.radiusSmall
                                            color: eyeMouse.containsMouse
                                                   ? Theme.surfaceHover
                                                   : Theme.surfaceRaised
                                            border.width: 1
                                            border.color: Theme.border
                                            AppIcon {
                                                anchors.centerIn: parent
                                                source:
                                                    widgetCard.modelData.window.visible
                                                    ? "qrc:/qt/qml/VCPilot/assets/icons/eye.svg"
                                                    : "qrc:/qt/qml/VCPilot/assets/icons/eye-off.svg"
                                                iconSize: 18
                                                iconColor:
                                                    eyeMouse.containsMouse
                                                    ? Theme.primary
                                                    : Theme.textSecondary
                                            }
                                            MouseArea {
                                                id: eyeMouse
                                                anchors.fill: parent
                                                hoverEnabled: true
                                                cursorShape: Qt.PointingHandCursor
                                                onClicked: {
                                                    const widget = widgetCard.modelData.window
                                                    if (widget.visible) {
                                                        widget.hide()
                                                    } else {
                                                        widget.show()
                                                        widget.raise()
                                                    }
                                                    root.saveWidgets()
                                                }
                                            }
                                        }
                                        // Remove
                                        Rectangle {
                                            width: 36
                                            height: 36
                                            radius: Theme.radiusSmall
                                            color: removeMouse.containsMouse
                                                   ? Theme.surfaceHover
                                                   : Theme.surfaceRaised
                                            border.width: 1
                                            border.color: Theme.border
                                            AppIcon {
                                                anchors.centerIn: parent
                                                source:
                                                    "qrc:/qt/qml/VCPilot/assets/icons/trash-2.svg"
                                                iconSize: 18
                                                iconColor:
                                                    removeMouse.containsMouse
                                                    ? "#EF4444"
                                                    : Theme.textSecondary
                                            }
                                            MouseArea {
                                                id: removeMouse
                                                anchors.fill: parent
                                                hoverEnabled: true
                                                cursorShape: Qt.PointingHandCursor
                                                onClicked: {
                                                    root.removeWidget(
                                                        widgetCard.modelData.window
                                                    )
                                                }
                                            }
                                        }
                                    }
                                    // Widget preview
                                    QuickSwitchContent {
                                        id: preview
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: implicitHeight
                                        previewMode: true
                                        widgetName:
                                            widgetCard.modelData.window.widgetName
                                        monitorId:
                                            widgetCard.modelData.window.monitorId
                                        monitorName:
                                            widgetCard.modelData.window.monitorName
                                        sources:
                                            widgetCard.modelData.window.sources
                                    }
                                }
                            }
                        }
                    }
                    }
                }
            }
        }
    }
}
