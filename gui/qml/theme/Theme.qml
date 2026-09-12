pragma Singleton

import QtQuick

QtObject {

    // Color Mode

    // true  -> Dark
    // false -> Light
    property bool darkMode: true

    // Colors
    readonly property color background:
        darkMode ? "#121212" : "#FFFFFF"

    readonly property color surface:
        darkMode ? "#1E1E1E" : "#F8F9FA"

    readonly property color surfaceRaised:
        darkMode ? "#252525" : "#F1F3F5"

    readonly property color surfaceHover:
        darkMode ? "#2C2C2C" : "#E9ECEF"

    // Borders
    readonly property color border:
        darkMode ? "#3A3A3A" : "#DEE2E6"

    readonly property color borderStrong:
        darkMode ? "#4F4F4F" : "#CED4DA"

    // Brand
    readonly property color primary: "#4A6CF7"

    readonly property color primaryHover:
        darkMode ? "#6483FF" : "#3A5CE0"

    readonly property color primaryPressed:
        darkMode ? "#3A5CE0" : "#2E4BC7"

    readonly property color accent: "#7C4DFF"

    // Text
    readonly property color textPrimary:
        darkMode ? "#FFFFFF" : "#1A1A1A"

    readonly property color textSecondary:
        darkMode ? "#B3B3B3" : "#495057"

    readonly property color textMuted:
        darkMode ? "#7A7A7A" : "#868E96"

    // Semantic
    readonly property color success:
        darkMode ? "#4CAF50" : "#2E7D32"

    readonly property color warning:
        darkMode ? "#FFB74D" : "#E65100"

    readonly property color danger:
        darkMode ? "#EF5350" : "#C62828"

    // Controls
    readonly property color controlHandle:
        darkMode ? "#FFFFFF" : "#FFFFFF"

    readonly property color controlHandlePressed:
        darkMode ? "#D0D0D0" : "#DEE2E6"

    readonly property color controlTrack:
        darkMode ? "#3A3A3A" : "#CED4DA"

    // Spacing
    readonly property int spacingXs: 4
    readonly property int spacingSm: 8
    readonly property int spacingMd: 12
    readonly property int spacingLg: 16
    readonly property int spacingXl: 24
    readonly property int spacingXxl: 32
    
    // Radius
    readonly property int radiusSmall: 8
    readonly property int radiusMedium: 12
    readonly property int radiusLarge: 16
    readonly property int radiusXLarge: 20

    // Typography
    readonly property int fontXs: 11
    readonly property int fontSm: 13
    readonly property int fontMd: 15
    readonly property int fontLg: 18
    readonly property int fontXl: 24
    readonly property int fontDisplay: 32

    readonly property int fontWeightNormal: Font.Normal
    readonly property int fontWeightMedium: Font.Medium
    readonly property int fontWeightBold: Font.Bold

    // Animation
    readonly property int animationFast: 120
    readonly property int animationNormal: 180
    readonly property int animationSlow: 260
}