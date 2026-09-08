#pragma once

#include "vcpilot/error.hpp"
#include "vcpilot/iddc_backend.hpp"

#include <physicalmonitorenumerationapi.h>
#include <windows.h>

namespace vcpilot {

class WindowsDdcBackend final : public IDdcBackend {
  public:
    ~WindowsDdcBackend() override;

    Result<std::vector<MonitorInfo>> listMonitors() override;

    Result<VcpValue> getVcp(const std::string& monitorId, std::uint8_t code) override;
    Result<void> setVcp(const std::string& monitorId, std::uint8_t code,
                        std::uint16_t value) override;

  private:
    struct MonitorHandleEntry {
        std::string id;
        HMONITOR logicalHandle;
        std::vector<PHYSICAL_MONITOR> physicalMonitors;
    };

    struct EnumerationContext {
        std::vector<MonitorInfo> monitors;
        std::vector<MonitorHandleEntry> handles;
    };

    static BOOL CALLBACK monitorEnumProc(HMONITOR monitorHandle, HDC monitorDc, LPRECT monitorRect,
                                         LPARAM data);

    void clearMonitorHandles();
    void destroyMonitorHandles(std::vector<MonitorHandleEntry>& handles);

    std::vector<MonitorHandleEntry> m_monitorHandles;
};

} // namespace vcpilot