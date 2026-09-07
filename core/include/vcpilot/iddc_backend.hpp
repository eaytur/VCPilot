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
};

} // namespace vcpilot
