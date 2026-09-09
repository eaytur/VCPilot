#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace vcpilot {

struct VcpCapability {
    std::uint8_t code;
    std::vector<std::uint16_t> values;
};

struct MonitorCapabilities {
    std::vector<VcpCapability> vcpFeatures;
    std::string mccsVersion;
};

MonitorCapabilities parseCapabilitiesString(const std::string& capabilities);

} // namespace vcpilot