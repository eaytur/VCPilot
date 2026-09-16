import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import VCPilot

SectionCard {
    id: root

    property string monitorId: ""
    property string output:
        "VCP Terminal\n" +
        "Type 'help' for available commands.\n"

    Layout.fillWidth: true
    Layout.fillHeight: true

    function appendOutput(text) {
        output += "\n" + text
    }

    function executeCommand(command) {
        const trimmed = command.trim()

        if (trimmed.length === 0)
            return

        appendOutput("> " + trimmed)

        const parts = trimmed.split(/\s+/)
        const commandName = parts[0].toLowerCase()

        if (commandName === "clear") {
            output = ""
            return
        }

        if (commandName === "help") {
            appendOutput(
                "getvcp <code>\n" +
                "setvcp <code> <value>\n" +
                "clear\n" +
                "help"
            )
            return
        }

        if (root.monitorId.length === 0) {
            appendOutput("Error: No monitor selected")
            return
        }

        if (commandName === "getvcp") {
            if (parts.length !== 2) {
                appendOutput("Usage: getvcp <code>")
                return
            }

            const code = parseInt(parts[1], 16)

            if (isNaN(code)) {
                appendOutput("Error: Invalid VCP code")
                return
            }

            const result =
                VCPilotAdapter.getVcp(
                    root.monitorId,
                    code
                )

            if (!result.success) {
                appendOutput("Error: " + result.error)
                return
            }

            appendOutput(
                "Current: " + result.current +
                "  Maximum: " + result.maximum
            )

            return
        }

        if (commandName === "setvcp") {
            if (parts.length !== 3) {
                appendOutput(
                    "Usage: setvcp <code> <value>"
                )
                return
            }

            const code = parseInt(parts[1], 16)

            // İlk etapta value decimal.
            const value = parseInt(parts[2], 10)

            if (isNaN(code) || isNaN(value)) {
                appendOutput(
                    "Error: Invalid code or value"
                )
                return
            }

            const result =
                VCPilotAdapter.setVcp(
                    root.monitorId,
                    code,
                    value
                )

            if (!result.success) {
                appendOutput("Error: " + result.error)
                return
            }

            appendOutput(
                "OK: VCP 0x" +
                code.toString(16).toUpperCase() +
                " set to " +
                value
            )

            return
        }

        appendOutput(
            "Unknown command: " + commandName
        )
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        spacing: Theme.spacingMd

        RowLayout {
            Layout.fillWidth: true

            AppIcon {
                source:
                    "qrc:/qt/qml/VCPilot/assets/icons/terminal.svg"

                iconSize: 18
                iconColor: Theme.primary
            }

            Text {
                text: "VCP Terminal"

                color: Theme.textPrimary
                font.pixelSize: Theme.fontMd
                font.weight: Theme.fontWeightBold
            }

            Item {
                Layout.fillWidth: true
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            radius: Theme.radiusSmall
            color: Theme.background

            border.width: 1
            border.color: Theme.border

            ScrollView {
                anchors.fill: parent
                anchors.margins: Theme.spacingMd

                TextArea {
                    id: terminalOutput

                    text: root.output

                    readOnly: true
                    selectByMouse: true
                    wrapMode: TextEdit.Wrap

                    color: Theme.textPrimary
                    selectionColor: Theme.primary

                    background: null

                    font.family: "Consolas"
                    font.pixelSize: Theme.fontSm
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true

            spacing: Theme.spacingSm

            Text {
                text: ">"

                color: Theme.primary
                font.family: "Consolas"
                font.pixelSize: Theme.fontMd
                font.weight: Theme.fontWeightBold
            }

            TextField {
                id: commandInput

                Layout.fillWidth: true

                placeholderText: "getvcp 10"

                font.family: "Consolas"

                onAccepted: {
                    root.executeCommand(text)

                    text = ""
                }
            }

            IconButton {
                iconSource:
                    "qrc:/qt/qml/VCPilot/assets/icons/send.svg"

                onClicked: {
                    root.executeCommand(
                        commandInput.text
                    )

                    commandInput.text = ""
                }
            }
        }
    }
}