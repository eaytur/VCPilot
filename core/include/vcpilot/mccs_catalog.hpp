#pragma once

#include <cstdint>
#include <string_view>

namespace vcpilot {

enum class VcpAccess {
    ReadOnly,
    WriteOnly,
    ReadWrite,
};

enum class VcpFeatureType {
    Continuous,
    NonContinuousSimple,
    NonContinuousComplex,
    Table,
};

struct VcpFeatureDescriptor {
    std::uint8_t code;
    std::string_view name;
    VcpAccess access;
    VcpFeatureType type;
};

struct VcpValueDescriptor {
    std::uint16_t value;
    std::string_view name;
};

const VcpFeatureDescriptor* findVcpFeatureDescriptor(std::uint8_t code);

std::string_view toString(VcpAccess access);
std::string_view toString(VcpFeatureType type);

const VcpValueDescriptor* findVcpValueDescriptor(std::uint8_t code, std::uint16_t value);

} // namespace vcpilot