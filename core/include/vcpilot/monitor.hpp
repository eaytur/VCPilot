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
    std::string manufacturer;
    std::string model;
    std::string serial;

    bool isPrimary;

    MonitorBounds bounds;
};

} // namespace vcpilot