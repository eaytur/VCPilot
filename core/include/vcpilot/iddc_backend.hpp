#pragma once

#include "vcpilot/error.hpp"
#include "vcpilot/monitor.hpp"
#include "vcpilot/monitor_capabilities.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace vcpilot {

class IDdcBackend {

  public:
    virtual ~IDdcBackend() = default;

    virtual Result<std::vector<MonitorInfo>> getMonitorInfos() = 0;

    virtual Result<VcpValue> getVcp(const std::string& monitorId, std::uint8_t code) = 0;
    virtual Result<void> setVcp(const std::string& monitorId, std::uint8_t code,
                                std::uint16_t value) = 0;
    virtual Result<MonitorCapabilities> getCapabilities(const std::string& monitorId) = 0;
};

} // namespace vcpilot
