#pragma once

#include "vcpilot/error.hpp"
#include "vcpilot/iddc_backend.hpp"
#include "vcpilot/monitor.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace vcpilot {

class MonitorController {
  public:
    MonitorController();

    Result<std::vector<MonitorInfo>> listMonitors();

    Result<VcpValue> getBrightness(const std::string& monitorId);

    Result<void> setBrightness(const std::string& monitorId, std::uint16_t value);

    Result<InputSource> getInputSource(const std::string& monitorId);

    Result<void> setInputSource(const std::string& monitorId, InputSource source);

  private:
    std::unique_ptr<IDdcBackend> m_backend;
};

} // namespace vcpilot