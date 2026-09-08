#include "vcpilot/monitor_controller.hpp"

#include "platform/windows/windows_ddc_backend.hpp"

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
} // namespace vcpilot
