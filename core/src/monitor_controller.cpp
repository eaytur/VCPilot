#include "vcpilot/monitor_controller.hpp"

#include "vcpilot/logger.hpp"

#include "platform/windows/windows_ddc_backend.hpp"

#include <algorithm>

namespace vcpilot {

namespace mccs_vcp_codes {

constexpr std::uint8_t kBrightness = 0x10;
constexpr std::uint8_t kInputSource = 0x60;

constexpr std::uint16_t kVga1 = 0x01;
constexpr std::uint16_t kVga2 = 0x02;
constexpr std::uint16_t kDvi1 = 0x03;
constexpr std::uint16_t kDvi2 = 0x04;
constexpr std::uint16_t kDisplayPort1 = 0x0F;
constexpr std::uint16_t kDisplayPort2 = 0x10;
constexpr std::uint16_t kHdmi1 = 0x11;
constexpr std::uint16_t kHdmi2 = 0x12;
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
} // namespace

MonitorController::MonitorController() : m_backend(std::make_unique<WindowsDdcBackend>()) {}

MonitorController::MonitorController(std::unique_ptr<IDdcBackend> backend)
    : m_backend(std::move(backend)) {}

Result<std::vector<MonitorInfo>> MonitorController::listMonitors() {
    return m_backend->listMonitors();
}

Result<VcpValue> MonitorController::getBrightness(const std::string& monitorId) {
    return m_backend->getVcp(monitorId, mccs_vcp_codes::kBrightness);
}

Result<void> MonitorController::setBrightness(const std::string& monitorId, std::uint16_t value) {
    return m_backend->setVcp(monitorId, mccs_vcp_codes::kBrightness, value);
}

Result<InputSource> MonitorController::getInputSource(const std::string& monitorId) {
    auto result = m_backend->getVcp(monitorId, mccs_vcp_codes::kInputSource);

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
    return m_backend->setVcp(monitorId, mccs_vcp_codes::kInputSource, toMccsValue(source));
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

} // namespace vcpilot
