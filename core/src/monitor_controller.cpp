#include "vcpilot/monitor_controller.hpp"

#include "vcpilot/logger.hpp"

#include "platform/windows/windows_ddc_backend.hpp"

#include <algorithm>

namespace vcpilot {

namespace mccs_vcp_codes {

constexpr std::uint8_t kBrightness = 0x10;
constexpr std::uint8_t kContrast = 0x12;
constexpr std::uint8_t kRedGain = 0x16;
constexpr std::uint8_t kGreenGain = 0x18;
constexpr std::uint8_t kBlueGain = 0x1A;
constexpr std::uint8_t kInputSource = 0x60;
constexpr std::uint8_t kVolume = 0x62;
constexpr std::uint8_t kRedBlackLevel = 0x6C;
constexpr std::uint8_t kGreenBlackLevel = 0x6E;
constexpr std::uint8_t kBlueBlackLevel = 0x70;
constexpr std::uint8_t kSharpness = 0x87;
constexpr std::uint8_t kSaturation = 0x8A;
constexpr std::uint8_t kAudioMute = 0x8D;
constexpr std::uint8_t kPowerMode = 0xD6;
constexpr std::uint8_t kColorPreset = 0x14;
constexpr std::uint8_t kGamma = 0x72;

constexpr std::uint16_t kVga1 = 0x01;
constexpr std::uint16_t kVga2 = 0x02;
constexpr std::uint16_t kDvi1 = 0x03;
constexpr std::uint16_t kDvi2 = 0x04;
constexpr std::uint16_t kDisplayPort1 = 0x0F;
constexpr std::uint16_t kDisplayPort2 = 0x10;
constexpr std::uint16_t kHdmi1 = 0x11;
constexpr std::uint16_t kHdmi2 = 0x12;
constexpr std::uint16_t kAudioMuted = 0x01;
constexpr std::uint16_t kAudioUnmuted = 0x02;
constexpr std::uint16_t kPowerOn = 0x01;
constexpr std::uint16_t kPowerStandby = 0x02;
constexpr std::uint16_t kPowerSuspend = 0x03;
constexpr std::uint16_t kPowerOff = 0x04;
constexpr std::uint16_t kColorPresetSRgb = 0x01;
constexpr std::uint16_t kColorPresetDisplayNative = 0x02;
constexpr std::uint16_t kColorPreset4000K = 0x03;
constexpr std::uint16_t kColorPreset5000K = 0x04;
constexpr std::uint16_t kColorPreset6500K = 0x05;
constexpr std::uint16_t kColorPreset7500K = 0x06;
constexpr std::uint16_t kColorPreset8200K = 0x07;
constexpr std::uint16_t kColorPreset9300K = 0x08;
constexpr std::uint16_t kColorPreset10000K = 0x09;
constexpr std::uint16_t kColorPreset11500K = 0x0A;
constexpr std::uint16_t kColorPresetUser1 = 0x0B;
constexpr std::uint16_t kColorPresetUser2 = 0x0C;
constexpr std::uint16_t kColorPresetUser3 = 0x0D;

} // namespace mccs_vcp_codes

namespace {
constexpr std::uint16_t toMccsValue(InputSource source) {
    switch (source) {
    case InputSource::Vga1:
        return mccs_vcp_codes::kVga1;
    case InputSource::Vga2:
        return mccs_vcp_codes::kVga2;
    case InputSource::Dvi1:
        return mccs_vcp_codes::kDvi1;
    case InputSource::Dvi2:
        return mccs_vcp_codes::kDvi2;
    case InputSource::DisplayPort1:
        return mccs_vcp_codes::kDisplayPort1;
    case InputSource::DisplayPort2:
        return mccs_vcp_codes::kDisplayPort2;
    case InputSource::Hdmi1:
        return mccs_vcp_codes::kHdmi1;
    case InputSource::Hdmi2:
        return mccs_vcp_codes::kHdmi2;
    }
    return 0;
}

constexpr std::optional<InputSource> fromMccsValue(std::uint16_t value) {
    switch (value) {
    case mccs_vcp_codes::kVga1:
        return InputSource::Vga1;
    case mccs_vcp_codes::kVga2:
        return InputSource::Vga2;
    case mccs_vcp_codes::kDvi1:
        return InputSource::Dvi1;
    case mccs_vcp_codes::kDvi2:
        return InputSource::Dvi2;
    case mccs_vcp_codes::kDisplayPort1:
        return InputSource::DisplayPort1;
    case mccs_vcp_codes::kDisplayPort2:
        return InputSource::DisplayPort2;
    case mccs_vcp_codes::kHdmi1:
        return InputSource::Hdmi1;
    case mccs_vcp_codes::kHdmi2:
        return InputSource::Hdmi2;
    default:
        return std::nullopt;
    }
}

constexpr std::uint16_t toMccsValue(PowerMode mode) {
    switch (mode) {
    case PowerMode::On:
        return mccs_vcp_codes::kPowerOn;
    case PowerMode::Standby:
        return mccs_vcp_codes::kPowerStandby;
    case PowerMode::Suspend:
        return mccs_vcp_codes::kPowerSuspend;
    case PowerMode::Off:
        return mccs_vcp_codes::kPowerOff;
    }

    return 0;
}

constexpr std::optional<PowerMode> powerModeFromMccsValue(std::uint16_t value) {
    switch (value) {
    case mccs_vcp_codes::kPowerOn:
        return PowerMode::On;
    case mccs_vcp_codes::kPowerStandby:
        return PowerMode::Standby;
    case mccs_vcp_codes::kPowerSuspend:
        return PowerMode::Suspend;
    case mccs_vcp_codes::kPowerOff:
        return PowerMode::Off;
    default:
        return std::nullopt;
    }
}

constexpr std::uint16_t toMccsValue(ColorPreset preset) {
    switch (preset) {
    case ColorPreset::SRgb:
        return mccs_vcp_codes::kColorPresetSRgb;
    case ColorPreset::DisplayNative:
        return mccs_vcp_codes::kColorPresetDisplayNative;
    case ColorPreset::K4000:
        return mccs_vcp_codes::kColorPreset4000K;
    case ColorPreset::K5000:
        return mccs_vcp_codes::kColorPreset5000K;
    case ColorPreset::K6500:
        return mccs_vcp_codes::kColorPreset6500K;
    case ColorPreset::K7500:
        return mccs_vcp_codes::kColorPreset7500K;
    case ColorPreset::K8200:
        return mccs_vcp_codes::kColorPreset8200K;
    case ColorPreset::K9300:
        return mccs_vcp_codes::kColorPreset9300K;
    case ColorPreset::K10000:
        return mccs_vcp_codes::kColorPreset10000K;
    case ColorPreset::K11500:
        return mccs_vcp_codes::kColorPreset11500K;
    case ColorPreset::User1:
        return mccs_vcp_codes::kColorPresetUser1;
    case ColorPreset::User2:
        return mccs_vcp_codes::kColorPresetUser2;
    case ColorPreset::User3:
        return mccs_vcp_codes::kColorPresetUser3;
    }

    return 0;
}

constexpr std::optional<ColorPreset> colorPresetFromMccsValue(std::uint16_t value) {

    switch (value) {
    case mccs_vcp_codes::kColorPresetSRgb:
        return ColorPreset::SRgb;
    case mccs_vcp_codes::kColorPresetDisplayNative:
        return ColorPreset::DisplayNative;
    case mccs_vcp_codes::kColorPreset4000K:
        return ColorPreset::K4000;
    case mccs_vcp_codes::kColorPreset5000K:
        return ColorPreset::K5000;
    case mccs_vcp_codes::kColorPreset6500K:
        return ColorPreset::K6500;
    case mccs_vcp_codes::kColorPreset7500K:
        return ColorPreset::K7500;
    case mccs_vcp_codes::kColorPreset8200K:
        return ColorPreset::K8200;
    case mccs_vcp_codes::kColorPreset9300K:
        return ColorPreset::K9300;
    case mccs_vcp_codes::kColorPreset10000K:
        return ColorPreset::K10000;
    case mccs_vcp_codes::kColorPreset11500K:
        return ColorPreset::K11500;
    case mccs_vcp_codes::kColorPresetUser1:
        return ColorPreset::User1;
    case mccs_vcp_codes::kColorPresetUser2:
        return ColorPreset::User2;
    case mccs_vcp_codes::kColorPresetUser3:
        return ColorPreset::User3;
    default:
        return std::nullopt;
    }
}

} // namespace

MonitorController::MonitorController() : m_backend(std::make_unique<WindowsDdcBackend>()) {}

MonitorController::MonitorController(std::unique_ptr<IDdcBackend> backend)
    : m_backend(std::move(backend)) {}

Result<std::vector<MonitorInfo>> MonitorController::listMonitors() {
    return m_backend->listMonitors();
}

Result<MonitorCapabilities> MonitorController::getCapabilities(const std::string& monitorId) {
    return m_backend->getCapabilities(monitorId);
}

Result<std::vector<Monitor>> MonitorController::getMonitors() {

    auto monitorInfos = listMonitors();

    if (!monitorInfos) {
        return std::unexpected(monitorInfos.error());
    }

    for (auto it = m_capabilitiesCache.begin(); it != m_capabilitiesCache.end();) {

        const auto monitorExists =
            std::find_if(monitorInfos->begin(), monitorInfos->end(),
                         [&it](const MonitorInfo& info) { return info.id == it->first; });

        if (monitorExists != monitorInfos->end()) {
            ++it;
        } else {
            VCPLOG_TRACE("Removing capabilities cache entry for disconnected monitor '{}'",
                         it->first);

            it = m_capabilitiesCache.erase(it);
        }
    }

    std::vector<Monitor> monitors;
    monitors.reserve(monitorInfos->size());

    for (auto& info : *monitorInfos) {

        auto cached = m_capabilitiesCache.find(info.id);

        if (cached != m_capabilitiesCache.end()) {

            VCPLOG_TRACE("Using cached capabilities for monitor '{}'", info.id);

            monitors.push_back(Monitor{
                .info = std::move(info),
                .capabilities = cached->second,
            });

            continue;
        }

        auto capabilities = getCapabilities(info.id);

        if (capabilities) {

            VCPLOG_TRACE("Caching capabilities for monitor '{}'", info.id);

            m_capabilitiesCache.emplace(info.id, *capabilities);

            monitors.push_back(Monitor{
                .info = std::move(info),
                .capabilities = std::move(*capabilities),
            });

        } else {

            VCPLOG_DEBUG("Capabilities unavailable for monitor '{}': {}", info.id,
                         capabilities.error().message);

            m_capabilitiesCache.emplace(info.id, std::nullopt);

            monitors.push_back(Monitor{
                .info = std::move(info),
                .capabilities = std::nullopt,
            });
        }
    }

    return monitors;
}

Result<VcpValue> MonitorController::getVcp(const std::string& monitorId, std::uint8_t code) {

    return m_backend->getVcp(monitorId, code);
}

Result<void> MonitorController::setVcp(const std::string& monitorId, std::uint8_t code,
                                       std::uint16_t value) {

    return m_backend->setVcp(monitorId, code, value);
}

Result<VcpValue> MonitorController::getBrightness(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kBrightness);
}

Result<void> MonitorController::setBrightness(const std::string& monitorId, std::uint16_t value) {
    return setVcp(monitorId, mccs_vcp_codes::kBrightness, value);
}

Result<InputSource> MonitorController::getInputSource(const std::string& monitorId) {
    auto result = getVcp(monitorId, mccs_vcp_codes::kInputSource);

    if (!result) {
        return std::unexpected(result.error());
    }

    auto source = fromMccsValue(result->current);

    if (!source) {
        return std::unexpected(Error{
            .code = ErrorCode::UnsupportedInputSource,
            .message = "Monitor reported an unsupported input source",
            .nativeCode = std::nullopt,
        });
    }

    return *source;
}

Result<void> MonitorController::setInputSource(const std::string& monitorId, InputSource source) {
    return setVcp(monitorId, mccs_vcp_codes::kInputSource, toMccsValue(source));
}

Result<VcpValue> MonitorController::getContrast(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kContrast);
}

Result<void> MonitorController::setContrast(const std::string& monitorId, std::uint16_t value) {
    return setVcp(monitorId, mccs_vcp_codes::kContrast, value);
}

Result<VcpValue> MonitorController::getVolume(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kVolume);
}

Result<void> MonitorController::setVolume(const std::string& monitorId, std::uint16_t value) {
    return setVcp(monitorId, mccs_vcp_codes::kVolume, value);
}

Result<VcpValue> MonitorController::getSharpness(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kSharpness);
}

Result<void> MonitorController::setSharpness(const std::string& monitorId, std::uint16_t value) {
    return setVcp(monitorId, mccs_vcp_codes::kSharpness, value);
}

Result<VcpValue> MonitorController::getSaturation(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kSaturation);
}

Result<void> MonitorController::setSaturation(const std::string& monitorId, std::uint16_t value) {
    return setVcp(monitorId, mccs_vcp_codes::kSaturation, value);
}

Result<VcpValue> MonitorController::getRedGain(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kRedGain);
}

Result<void> MonitorController::setRedGain(const std::string& monitorId, std::uint16_t value) {
    return setVcp(monitorId, mccs_vcp_codes::kRedGain, value);
}

Result<VcpValue> MonitorController::getGreenGain(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kGreenGain);
}

Result<void> MonitorController::setGreenGain(const std::string& monitorId, std::uint16_t value) {
    return setVcp(monitorId, mccs_vcp_codes::kGreenGain, value);
}

Result<VcpValue> MonitorController::getBlueGain(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kBlueGain);
}

Result<void> MonitorController::setBlueGain(const std::string& monitorId, std::uint16_t value) {
    return setVcp(monitorId, mccs_vcp_codes::kBlueGain, value);
}

Result<VcpValue> MonitorController::getRedBlackLevel(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kRedBlackLevel);
}

Result<void> MonitorController::setRedBlackLevel(const std::string& monitorId,
                                                 std::uint16_t value) {
    return setVcp(monitorId, mccs_vcp_codes::kRedBlackLevel, value);
}

Result<VcpValue> MonitorController::getGreenBlackLevel(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kGreenBlackLevel);
}

Result<void> MonitorController::setGreenBlackLevel(const std::string& monitorId,
                                                   std::uint16_t value) {
    return setVcp(monitorId, mccs_vcp_codes::kGreenBlackLevel, value);
}

Result<VcpValue> MonitorController::getBlueBlackLevel(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kBlueBlackLevel);
}

Result<void> MonitorController::setBlueBlackLevel(const std::string& monitorId,
                                                  std::uint16_t value) {
    return setVcp(monitorId, mccs_vcp_codes::kBlueBlackLevel, value);
}

Result<bool> MonitorController::getMute(const std::string& monitorId) {
    auto result = getVcp(monitorId, mccs_vcp_codes::kAudioMute);

    if (!result) {
        return std::unexpected(result.error());
    }

    switch (result->current) {
    case mccs_vcp_codes::kAudioMuted:
        return true;

    case mccs_vcp_codes::kAudioUnmuted:
        return false;

    default:
        return std::unexpected(Error{
            .code = ErrorCode::Unknown,
            .message = "Monitor reported an unsupported audio mute state",
            .nativeCode = std::nullopt,
        });
    }
}

Result<void> MonitorController::setMute(const std::string& monitorId, bool muted) {

    const auto value = muted ? mccs_vcp_codes::kAudioMuted : mccs_vcp_codes::kAudioUnmuted;

    return setVcp(monitorId, mccs_vcp_codes::kAudioMute, value);
}

Result<PowerMode> MonitorController::getPowerMode(const std::string& monitorId) {
    auto result = getVcp(monitorId, mccs_vcp_codes::kPowerMode);

    if (!result) {
        return std::unexpected(result.error());
    }

    auto mode = powerModeFromMccsValue(result->current);

    if (!mode) {
        return std::unexpected(Error{
            .code = ErrorCode::Unknown,
            .message = "Monitor reported an unsupported power mode",
            .nativeCode = std::nullopt,
        });
    }

    return *mode;
}

Result<void> MonitorController::setPowerMode(const std::string& monitorId, PowerMode mode) {

    return setVcp(monitorId, mccs_vcp_codes::kPowerMode, toMccsValue(mode));
}

Result<ColorPreset> MonitorController::getColorPreset(const std::string& monitorId) {

    auto result = getVcp(monitorId, mccs_vcp_codes::kColorPreset);

    if (!result) {
        return std::unexpected(result.error());
    }

    auto preset = colorPresetFromMccsValue(result->current);

    if (!preset) {
        return std::unexpected(Error{
            .code = ErrorCode::Unknown,
            .message = "Monitor reported an unsupported color preset",
            .nativeCode = std::nullopt,
        });
    }

    return *preset;
}

Result<void> MonitorController::setColorPreset(const std::string& monitorId, ColorPreset preset) {

    return setVcp(monitorId, mccs_vcp_codes::kColorPreset, toMccsValue(preset));
}

Result<VcpValue> MonitorController::getGamma(const std::string& monitorId) {
    return getVcp(monitorId, mccs_vcp_codes::kGamma);
}

Result<void> MonitorController::setGamma(const std::string& monitorId, std::uint16_t value) {

    return setVcp(monitorId, mccs_vcp_codes::kGamma, value);
}

} // namespace vcpilot
