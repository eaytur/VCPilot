#pragma once

#include "vcpilot/error.hpp"
#include "vcpilot/iddc_backend.hpp"

namespace vcpilot {

class WindowsDdcBackend final : public IDdcBackend {
  public:
    Result<std::vector<MonitorInfo>> listMonitors() override;
};

} // namespace vcpilot