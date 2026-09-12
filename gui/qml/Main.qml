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

    property int currentPage: 0


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
                }
            ]

            onNavigationRequested: function(index) {
                window.currentPage = index
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: window.currentPage


            DashboardPage {
            }
        }
    }
}