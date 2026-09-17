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

    property var commandHistory: []
    property int historyIndex: commandHistory.length

    Layout.fillWidth: true
    Layout.fillHeight: true

    function appendOutput(text) {
        output += "\n" + text
    }

    function parseVcpCode(text) {
        const normalized = text.trim()

        if (normalized.length === 0)
            return NaN

        if (normalized.toLowerCase().startsWith("0x"))
            return parseInt(normalized.substring(2), 16)

        return parseInt(normalized, 16)
    }

    function parseVcpValue(text) {
        const normalized = text.trim()

        if (normalized.length === 0)
            return NaN

        // Explicit 0x prefix -> hexadecimal.
        if (normalized.toLowerCase().startsWith("0x"))
            return parseInt(normalized.substring(2), 16)

        return parseInt(normalized, 10)
    }

    function addToHistory(command) {
        const trimmed = command.trim()

        if (trimmed.length === 0)
            return

        const updatedHistory = commandHistory.slice()

        if (updatedHistory.length === 0
                || updatedHistory[updatedHistory.length - 1] !== trimmed) {
            updatedHistory.push(trimmed)
        }

        const maxHistorySize = 50

        if (updatedHistory.length > maxHistorySize)
            updatedHistory.shift()

        commandHistory = updatedHistory
        historyIndex = commandHistory.length
    }

    function previousCommand() {
        if (commandHistory.length === 0)
            return

        if (historyIndex > 0)
            historyIndex--

        commandInput.text =
            commandHistory[historyIndex]

        commandInput.cursorPosition =
            commandInput.text.length
    }

    function nextCommand() {
        if (commandHistory.length === 0)
            return

        if (historyIndex < commandHistory.length - 1) {
            historyIndex++

            commandInput.text =
                commandHistory[historyIndex]
        } else {
            historyIndex =
                commandHistory.length

            commandInput.text = ""
        }

        commandInput.cursorPosition =
            commandInput.text.length
    }

    function submitCommand() {
        const command = commandInput.text.trim()

        if (command.length === 0)
            return

        addToHistory(command)
        executeCommand(command)

        commandInput.text = ""
        historyIndex = commandHistory.length
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
                "help\n\n" +
                "VCP codes are hexadecimal.\n" +
                "Values are decimal unless prefixed with 0x."
            )

            return
        }

        if (root.monitorId.length === 0) {
            appendOutput(
                "Error: No monitor selected"
            )

            return
        }

        if (commandName === "getvcp") {
            if (parts.length !== 2) {
                appendOutput(
                    "Usage: getvcp <code>"
                )

                return
            }

            const code =
                root.parseVcpCode(parts[1])

            if (isNaN(code)
                    || code < 0
                    || code > 0xFF) {

                appendOutput(
                    "Error: Invalid VCP code"
                )

                return
            }

            const result =
                VCPilotAdapter.getVcp(
                    root.monitorId,
                    code
                )

            if (!result.success) {
                appendOutput(
                    "Error: " + result.error
                )

                return
            }

            appendOutput(
                "Current: "
                + result.current
                + "  Maximum: "
                + result.maximum
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

            const code =
                root.parseVcpCode(parts[1])

            const value =
                root.parseVcpValue(parts[2])

            if (isNaN(code)
                    || code < 0
                    || code > 0xFF) {

                appendOutput(
                    "Error: Invalid VCP code"
                )

                return
            }

            if (isNaN(value)
                    || value < 0
                    || value > 0xFFFF) {

                appendOutput(
                    "Error: Invalid VCP value"
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
                appendOutput(
                    "Error: " + result.error
                )

                return
            }

            appendOutput(
                "OK: VCP "
                + root.codeString(code)
                + " set to "
                + value
                + " (0x"
                + value
                    .toString(16)
                    .toUpperCase()
                    .padStart(2, "0")
                + ")"
            )

            return
        }

        appendOutput(
            "Unknown command: "
            + commandName
        )
    }

    function codeString(code) {
        return "0x"
            + Number(code)
                .toString(16)
                .toUpperCase()
                .padStart(2, "0")
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

                    onTextChanged: {
                        cursorPosition = length
                    }
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

            AppTextField {
                id: commandInput

                Layout.fillWidth: true

                placeholderText: "getvcp 10"
                font.family: "Consolas"

                Keys.onUpPressed: function(event) {
                    root.previousCommand()
                    event.accepted = true
                }

                Keys.onDownPressed: function(event) {
                    root.nextCommand()
                    event.accepted = true
                }

                onAccepted: {
                    root.submitCommand()
                }
            }

            IconButton {
                iconSource:
                    "qrc:/qt/qml/VCPilot/assets/icons/send.svg"

                onClicked: {
                    root.submitCommand()
                    commandInput.forceActiveFocus()
                }
            }
        }
    }
}