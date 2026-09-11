pragma Singleton

import QtQuick

QtObject {
    // Colors
    readonly property color background: "#0B1118"

    readonly property color surface: "#101821"
    readonly property color surfaceRaised: "#15202B"
    readonly property color surfaceHover: "#1A2734"

    readonly property color border: "#263646"
    readonly property color borderStrong: "#34506A"

    readonly property color primary: "#2596FF"
    readonly property color primaryHover: "#48A8FF"
    readonly property color primaryPressed: "#167DDA"

    readonly property color accent: "#5DD6FF"

    readonly property color textPrimary: "#F4F8FC"
    readonly property color textSecondary: "#A8B6C6"
    readonly property color textMuted: "#6F8296"

    readonly property color success: "#42D39B"
    readonly property color warning: "#F0B35E"
    readonly property color danger: "#ED6A6A"

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