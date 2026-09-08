#pragma once

#include <cstdint>
#include <string>

namespace vcpilot {

struct MonitorBounds {

    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
};

struct MonitorInfo {
    std::string id;
    std::string manufacturer;
    std::string model;
    std::string serial;

    bool isPrimary;

    MonitorBounds bounds;
};

struct VcpValue {
    std::uint16_t current;
    std::uint16_t maximum;
};

enum class InputSource { Vga1, Vga2, Dvi1, Dvi2, DisplayPort1, DisplayPort2, Hdmi1, Hdmi2 };

constexpr std::string_view toString(InputSource source) {
    switch (source) {
    case InputSource::Vga1:
        return "Vga1";
    case InputSource::Vga2:
        return "Vga2";
    case InputSource::Dvi1:
        return "Dvi1";
    case InputSource::Dvi2:
        return "Dvi2";
    case InputSource::DisplayPort1:
        return "DisplayPort1";
    case InputSource::DisplayPort2:
        return "DisplayPort2";
    case InputSource::Hdmi1:
        return "Hdmi1";
    case InputSource::Hdmi2:
        return "Hdmi2";
    }

    return "Unknown";
}

} // namespace vcpilot