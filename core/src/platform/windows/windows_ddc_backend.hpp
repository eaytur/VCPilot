#pragma once

#include "vcpilot/error.hpp"
#include "vcpilot/iddc_backend.hpp"
#include "vcpilot/monitor_capabilities.hpp"

#include <physicalmonitorenumerationapi.h>
#include <windows.h>

namespace vcpilot {

class WindowsDdcBackend final : public IDdcBackend {
  public:
    ~WindowsDdcBackend() override;

    Result<std::vector<MonitorInfo>> getMonitorInfos() override;

    Result<VcpValue> getVcp(const std::string& monitorId, std::uint8_t code) override;
    Result<void> setVcp(const std::string& monitorId, std::uint8_t code,
                        std::uint16_t value) override;
    Result<MonitorCapabilities> getCapabilities(const std::string& monitorId) override;

  private:
    struct MonitorHandleEntry {
        std::string id;
        std::vector<PHYSICAL_MONITOR> physicalMonitors;
    };

    struct EnumerationContext {
        std::vector<MonitorInfo> monitors;
        std::vector<MonitorHandleEntry> handles;
    };

    static BOOL CALLBACK monitorEnumProc(HMONITOR monitorHandle, HDC monitorDc, LPRECT monitorRect,
                                         LPARAM data);

    Result<EnumerationContext> enumerateMonitors();

    Result<void> refreshMonitorHandles();

    Result<VcpValue> getVcpInternal(const std::string& monitorId, std::uint8_t code);

    Result<void> setVcpInternal(const std::string& monitorId, std::uint8_t code,
                                std::uint16_t value);
    Result<MonitorCapabilities> getCapabilitiesInternal(const std::string& monitorId);
    void clearMonitorHandles();
    void destroyMonitorHandles(std::vector<MonitorHandleEntry>& handles);

    std::vector<MonitorHandleEntry> m_monitorHandles;
};

} // namespace vcpilot