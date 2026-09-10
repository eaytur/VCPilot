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

    Result<MonitorCapabilities> getCapabilities(const std::string& monitorId);

    Result<std::vector<Monitor>> getMonitors();

    Result<VcpValue> getVcp(const std::string& monitorId, std::uint8_t code);

    Result<void> setVcp(const std::string& monitorId, std::uint8_t code, std::uint16_t value);

    Result<VcpValue> getBrightness(const std::string& monitorId);
    Result<void> setBrightness(const std::string& monitorId, std::uint16_t value);

    Result<InputSource> getInputSource(const std::string& monitorId);
    Result<void> setInputSource(const std::string& monitorId, InputSource source);

    Result<VcpValue> getContrast(const std::string& monitorId);
    Result<void> setContrast(const std::string& monitorId, std::uint16_t value);

    Result<VcpValue> getVolume(const std::string& monitorId);
    Result<void> setVolume(const std::string& monitorId, std::uint16_t value);

    Result<VcpValue> getSharpness(const std::string& monitorId);
    Result<void> setSharpness(const std::string& monitorId, std::uint16_t value);

    Result<VcpValue> getSaturation(const std::string& monitorId);
    Result<void> setSaturation(const std::string& monitorId, std::uint16_t value);

    Result<VcpValue> getRedGain(const std::string& monitorId);
    Result<void> setRedGain(const std::string& monitorId, std::uint16_t value);

    Result<VcpValue> getGreenGain(const std::string& monitorId);
    Result<void> setGreenGain(const std::string& monitorId, std::uint16_t value);

    Result<VcpValue> getBlueGain(const std::string& monitorId);
    Result<void> setBlueGain(const std::string& monitorId, std::uint16_t value);

    Result<VcpValue> getRedBlackLevel(const std::string& monitorId);
    Result<void> setRedBlackLevel(const std::string& monitorId, std::uint16_t value);

    Result<VcpValue> getGreenBlackLevel(const std::string& monitorId);
    Result<void> setGreenBlackLevel(const std::string& monitorId, std::uint16_t value);

    Result<VcpValue> getBlueBlackLevel(const std::string& monitorId);
    Result<void> setBlueBlackLevel(const std::string& monitorId, std::uint16_t value);

    Result<bool> getMute(const std::string& monitorId);
    Result<void> setMute(const std::string& monitorId, bool muted);

    Result<PowerMode> getPowerMode(const std::string& monitorId);
    Result<void> setPowerMode(const std::string& monitorId, PowerMode mode);

    Result<ColorPreset> getColorPreset(const std::string& monitorId);
    Result<void> setColorPreset(const std::string& monitorId, ColorPreset preset);

    Result<VcpValue> getGamma(const std::string& monitorId);
    Result<void> setGamma(const std::string& monitorId, std::uint16_t value);

  private:
    std::unique_ptr<IDdcBackend> m_backend;
    std::unordered_map<std::string, std::optional<MonitorCapabilities>> m_capabilitiesCache;
};

} // namespace vcpilot