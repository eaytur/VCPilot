import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

ApplicationWindow {
    id: window

    width: 1400
    height: 900

    minimumWidth: 1000
    minimumHeight: 650

    visible: true

    color: Theme.background

    property int currentPage: Main.Dashboard
    property string selectedMonitorId: ""

    enum Page {
        Dashboard,
        Controls
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        AppHeader {
            Layout.fillWidth: true
            Layout.preferredHeight: 72

            currentIndex: window.currentPage

            navigationItems: [
                {
                    text: "Dashboard",
                    icon: "qrc:/qt/qml/VCPilot/assets/icons/layout-dashboard.svg"
                },
                {
                    text: "Controls",
                    icon: "qrc:/qt/qml/VCPilot/assets/icons/controls.svg"
                }
            ]

            onNavigationRequested: function(index) {
                window.currentPage = index
            }
        }

        MonitorStrip {
            Layout.fillWidth: true
            Layout.preferredHeight: 190

            monitors: VCPilotAdapter.monitors

            selectedMonitorId: window.selectedMonitorId

            onMonitorSelected: function(monitorId) {
                window.selectedMonitorId = monitorId
            }

            onRefreshRequested: {
                VCPilotAdapter.refreshMonitors()
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: window.currentPage

            DashboardPage {
                selectedMonitorId: window.selectedMonitorId

                onMonitorSelected: function(monitorId) {
                    window.selectedMonitorId = monitorId
                }
                onControlRequested: {
                    window.currentPage = Main.Controls
                }
            }
            ControlsPage {
                selectedMonitorId: window.selectedMonitorId
            }
        }

        AppFooter {
            Layout.fillWidth: true
            Layout.preferredHeight: 68

            selectedMonitorId: window.selectedMonitorId
        }
    }

    Component.onCompleted: {
        VCPilotAdapter.refreshMonitors()
    }
}