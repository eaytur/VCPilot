// AppTextField.qml

import QtQuick
import QtQuick.Controls

TextField {
    id: root

    property color backgroundColor: Theme.background
    property color borderColor: activeFocus
                                ? Theme.primary
                                : Theme.border

    leftPadding: Theme.spacingMd
    rightPadding: Theme.spacingMd

    color: Theme.textPrimary
    placeholderTextColor: Theme.textSecondary
    selectionColor: Theme.primary
    selectedTextColor: Theme.textPrimary

    font.pixelSize: Theme.fontSm

    background: Rectangle {
        radius: Theme.radiusMedium

        color: root.backgroundColor

        border.width: root.activeFocus ? 2 : 1
        border.color: root.borderColor
    }
}