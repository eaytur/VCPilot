#pragma once

#include "vcpilot/error.hpp"
#include "vcpilot/iddc_backend.hpp"
#include "vcpilot/monitor.hpp"
#include "vcpilot/monitor_capabilities.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace vcpilot {

class MonitorController {
  public:
    MonitorController();

    explicit MonitorController(std::unique_ptr<IDdcBackend> backend);

    Result<std::vector<MonitorInfo>> listMonitors();

    Result<VcpValue> getBrightness(const std::string& monitorId);

    Result<void> setBrightness(const std::string& monitorId, std::uint16_t value);

    Result<InputSource> getInputSource(const std::string& monitorId);

    Result<void> setInputSource(const std::string& monitorId, InputSource source);

    Result<MonitorCapabilities> getCapabilities(const std::string& monitorId);

    Result<std::vector<Monitor>> getMonitors();

    Result<VcpValue> getVcp(const std::string& monitorId, std::uint8_t code);

    Result<void> setVcp(const std::string& monitorId, std::uint8_t code, std::uint16_t value);

  private:
    std::unique_ptr<IDdcBackend> m_backend;
    std::unordered_map<std::string, std::optional<MonitorCapabilities>> m_capabilitiesCache;
};

} // namespace vcpilot