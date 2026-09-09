#include "vcpilot/mccs_catalog.hpp"

#include <array>

namespace vcpilot {

namespace {

constexpr std::array kMccs20Features{
    VcpFeatureDescriptor{
        .code = 0x02,
        .name = "New Control Value",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousComplex,
    },
    VcpFeatureDescriptor{
        .code = 0x04,
        .name = "Restore Factory Defaults",
        .access = VcpAccess::WriteOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x05,
        .name = "Restore Factory Brightness/Contrast Defaults",
        .access = VcpAccess::WriteOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x06,
        .name = "Restore Factory Geometry Defaults",
        .access = VcpAccess::WriteOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x08,
        .name = "Restore Color Defaults",
        .access = VcpAccess::WriteOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x0A,
        .name = "Restore Factory TV Defaults",
        .access = VcpAccess::WriteOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x0B,
        .name = "Color Temperature Increment",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::NonContinuousComplex,
    },
    VcpFeatureDescriptor{
        .code = 0x0C,
        .name = "Color Temperature Request",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0x0E,
        .name = "Clock",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0x10,
        .name = "Brightness",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0x12,
        .name = "Contrast",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0x14,
        .name = "Select Color Preset",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x16,
        .name = "Video Gain: Red",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0x18,
        .name = "Video Gain: Green",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0x1A,
        .name = "Video Gain: Blue",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },

    VcpFeatureDescriptor{
        .code = 0x60,
        .name = "Input Source",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x62,
        .name = "Audio Speaker Volume",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },

    VcpFeatureDescriptor{
        .code = 0x6C,
        .name = "Video Black Level: Red",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0x6E,
        .name = "Video Black Level: Green",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0x70,
        .name = "Video Black Level: Blue",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },

    VcpFeatureDescriptor{
        .code = 0x73,
        .name = "LUT Size",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::Table,
    },
    VcpFeatureDescriptor{
        .code = 0x74,
        .name = "Single Point LUT Operation",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Table,
    },
    VcpFeatureDescriptor{
        .code = 0x76,
        .name = "Remote Procedure Call",
        .access = VcpAccess::WriteOnly,
        .type = VcpFeatureType::Table,
    },

    VcpFeatureDescriptor{
        .code = 0x7A,
        .name = "Adjust Focal Plane",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x7C,
        .name = "Adjust Zoom",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0x7E,
        .name = "Trapezoid",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0x80,
        .name = "Keystone",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0x82,
        .name = "Horizontal Mirror",
        .access = VcpAccess::WriteOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x84,
        .name = "Vertical Mirror",
        .access = VcpAccess::WriteOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x86,
        .name = "Display Scaling",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x87,
        .name = "Sharpness",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0x8D,
        .name = "Audio Mute",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousSimple,
    },

    VcpFeatureDescriptor{
        .code = 0xAC,
        .name = "Horizontal Frequency",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0xAE,
        .name = "Vertical Frequency",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::Continuous,
    },

    VcpFeatureDescriptor{
        .code = 0xB0,
        .name = "Settings",
        .access = VcpAccess::WriteOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0xB2,
        .name = "Flat Panel Sub-Pixel Layout",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0xB6,
        .name = "Display Technology Type",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },

    VcpFeatureDescriptor{
        .code = 0xC0,
        .name = "Display Usage Time",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0xC2,
        .name = "Display Descriptor Length",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::Continuous,
    },
    VcpFeatureDescriptor{
        .code = 0xC3,
        .name = "Transmit Display Descriptor",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::Table,
    },
    VcpFeatureDescriptor{
        .code = 0xC4,
        .name = "Enable Display Descriptor",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousComplex,
    },
    VcpFeatureDescriptor{
        .code = 0xC6,
        .name = "Application Enable Key",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::NonContinuousComplex,
    },
    VcpFeatureDescriptor{
        .code = 0xC8,
        .name = "Display Controller Type",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::NonContinuousComplex,
    },
    VcpFeatureDescriptor{
        .code = 0xC9,
        .name = "Display Firmware Level",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::NonContinuousComplex,
    },
    VcpFeatureDescriptor{
        .code = 0xCA,
        .name = "OSD/Button Control",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousComplex,
    },
    VcpFeatureDescriptor{
        .code = 0xCC,
        .name = "OSD Language",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0xCE,
        .name = "Auxiliary Display Size",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::NonContinuousComplex,
    },
    VcpFeatureDescriptor{
        .code = 0xCF,
        .name = "Auxiliary Display Data",
        .access = VcpAccess::WriteOnly,
        .type = VcpFeatureType::Table,
    },

    VcpFeatureDescriptor{
        .code = 0xD0,
        .name = "Output Select",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0xD6,
        .name = "Power Mode",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0xDA,
        .name = "Scan Mode",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0xDC,
        .name = "Display Mode",
        .access = VcpAccess::ReadWrite,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0xDE,
        .name = "Scratch Pad",
        .access = VcpAccess::WriteOnly,
        .type = VcpFeatureType::NonContinuousSimple,
    },
    VcpFeatureDescriptor{
        .code = 0xDF,
        .name = "VCP Version",
        .access = VcpAccess::ReadOnly,
        .type = VcpFeatureType::NonContinuousComplex,
    },
};

constexpr std::array kInputSourceValues{
    VcpValueDescriptor{0x01, "VGA 1"},
    VcpValueDescriptor{0x02, "VGA 2"},
    VcpValueDescriptor{0x03, "DVI 1"},
    VcpValueDescriptor{0x04, "DVI 2"},
    VcpValueDescriptor{0x05, "Composite Video 1"},
    VcpValueDescriptor{0x06, "Composite Video 2"},
    VcpValueDescriptor{0x07, "S-Video 1"},
    VcpValueDescriptor{0x08, "S-Video 2"},
    VcpValueDescriptor{0x09, "Tuner 1"},
    VcpValueDescriptor{0x0A, "Tuner 2"},
    VcpValueDescriptor{0x0B, "Tuner 3"},
    VcpValueDescriptor{0x0C, "Component Video 1"},
    VcpValueDescriptor{0x0D, "Component Video 2"},
    VcpValueDescriptor{0x0E, "Component Video 3"},
    VcpValueDescriptor{0x0F, "DisplayPort 1"},
    VcpValueDescriptor{0x10, "DisplayPort 2"},
    VcpValueDescriptor{0x11, "HDMI 1"},
    VcpValueDescriptor{0x12, "HDMI 2"},
};

constexpr std::array kColorPresetValues{
    VcpValueDescriptor{0x01, "sRGB"},   VcpValueDescriptor{0x02, "Display Native"},
    VcpValueDescriptor{0x03, "4000K"},  VcpValueDescriptor{0x04, "5000K"},
    VcpValueDescriptor{0x05, "6500K"},  VcpValueDescriptor{0x06, "7500K"},
    VcpValueDescriptor{0x07, "8200K"},  VcpValueDescriptor{0x08, "9300K"},
    VcpValueDescriptor{0x09, "10000K"}, VcpValueDescriptor{0x0A, "11500K"},
    VcpValueDescriptor{0x0B, "User 1"}, VcpValueDescriptor{0x0C, "User 2"},
    VcpValueDescriptor{0x0D, "User 3"},
};

constexpr std::array kPowerModeValues{
    VcpValueDescriptor{0x01, "On"},
    VcpValueDescriptor{0x02, "Standby"},
    VcpValueDescriptor{0x03, "Suspend"},
    VcpValueDescriptor{0x04, "Off"},
};

constexpr std::array kAudioMuteValues{
    VcpValueDescriptor{0x01, "Mute"},
    VcpValueDescriptor{0x02, "Unmute"},
};

constexpr std::array kOsdLanguageValues{
    VcpValueDescriptor{0x00, "Reserved"},
    VcpValueDescriptor{0x01, "Chinese (Traditional)"},
    VcpValueDescriptor{0x02, "English"},
    VcpValueDescriptor{0x03, "French"},
    VcpValueDescriptor{0x04, "German"},
    VcpValueDescriptor{0x05, "Italian"},
    VcpValueDescriptor{0x06, "Japanese"},
    VcpValueDescriptor{0x07, "Korean"},
    VcpValueDescriptor{0x08, "Portuguese (Portugal)"},
    VcpValueDescriptor{0x09, "Russian"},
    VcpValueDescriptor{0x0A, "Spanish"},
    VcpValueDescriptor{0x0B, "Swedish"},
    VcpValueDescriptor{0x0C, "Turkish"},
    VcpValueDescriptor{0x0D, "Chinese (Simplified)"},
    VcpValueDescriptor{0x0E, "Portuguese (Brazil)"},
    VcpValueDescriptor{0x0F, "Arabic"},
    VcpValueDescriptor{0x10, "Bulgarian"},
    VcpValueDescriptor{0x11, "Croatian"},
    VcpValueDescriptor{0x12, "Czech"},
    VcpValueDescriptor{0x13, "Danish"},
    VcpValueDescriptor{0x14, "Dutch"},
    VcpValueDescriptor{0x15, "Estonian"},
    VcpValueDescriptor{0x16, "Finnish"},
    VcpValueDescriptor{0x17, "Greek"},
    VcpValueDescriptor{0x18, "Hebrew"},
    VcpValueDescriptor{0x19, "Hindi"},
    VcpValueDescriptor{0x1A, "Hungarian"},
    VcpValueDescriptor{0x1B, "Latvian"},
    VcpValueDescriptor{0x1C, "Lithuanian"},
    VcpValueDescriptor{0x1D, "Norwegian"},
    VcpValueDescriptor{0x1E, "Polish"},
    VcpValueDescriptor{0x1F, "Romanian"},
    VcpValueDescriptor{0x20, "Serbian"},
    VcpValueDescriptor{0x21, "Slovak"},
    VcpValueDescriptor{0x22, "Slovenian"},
    VcpValueDescriptor{0x23, "Thai"},
    VcpValueDescriptor{0x24, "Ukrainian"},
};
} // namespace

const VcpFeatureDescriptor* findVcpFeatureDescriptor(std::uint8_t code) {

    for (const auto& feature : kMccs20Features) {
        if (feature.code == code) {
            return &feature;
        }
    }

    return nullptr;
}

std::string_view toString(VcpAccess access) {
    switch (access) {
    case VcpAccess::ReadOnly:
        return "Read Only";

    case VcpAccess::WriteOnly:
        return "Write Only";

    case VcpAccess::ReadWrite:
        return "Read/Write";
    }

    return "Unknown";
}

std::string_view toString(VcpFeatureType type) {
    switch (type) {
    case VcpFeatureType::Continuous:
        return "Continuous";

    case VcpFeatureType::NonContinuousSimple:
        return "Non-Continuous Simple";

    case VcpFeatureType::NonContinuousComplex:
        return "Non-Continuous Complex";

    case VcpFeatureType::Table:
        return "Table";
    }

    return "Unknown";
}

const VcpValueDescriptor* findVcpValueDescriptor(std::uint8_t code, std::uint16_t value) {

    const auto findValue = [value](const auto& values) -> const VcpValueDescriptor* {
        for (const auto& descriptor : values) {
            if (descriptor.value == value) {
                return &descriptor;
            }
        }

        return nullptr;
    };

    switch (code) {
    case 0x14:
        return findValue(kColorPresetValues);

    case 0x60:
        return findValue(kInputSourceValues);

    case 0x8D:
        return findValue(kAudioMuteValues);

    case 0xCC:
        return findValue(kOsdLanguageValues);

    case 0xD6:
        return findValue(kPowerModeValues);

    default:
        return nullptr;
    }
}

} // namespace vcpilot