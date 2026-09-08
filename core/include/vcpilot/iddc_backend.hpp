#pragma once

#include "vcpilot/error.hpp"
#include "vcpilot/monitor.hpp"

#include <cstdint>
#include <vector>

namespace vcpilot {

class IDdcBackend {

  public:
    virtual ~IDdcBackend() = default;

    virtual Result<std::vector<MonitorInfo>> listMonitors() = 0;

    virtual Result<VcpValue> getVcp(const std::string& monitorId, std::uint8_t code) = 0;
    virtual Result<void> setVcp(const std::string& monitorId, std::uint8_t code,
                                std::uint16_t value) = 0;
};

} // namespace vcpilot
