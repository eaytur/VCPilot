#include "vcpilot_cli/commands/vcp_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <charconv>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string_view>

namespace {

std::optional<std::uint32_t> parseNumber(std::string_view text) {
    int base = 10;

    if (text.starts_with("0x") || text.starts_with("0X")) {
        base = 16;
        text.remove_prefix(2);
    }

    if (text.empty()) {
        return std::nullopt;
    }

    std::uint32_t value{};

    const auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), value, base);

    if (ec != std::errc{} || ptr != text.data() + text.size()) {
        return std::nullopt;
    }

    return value;
}

} // namespace

namespace vcpilot_cli {

VcpCommand::VcpCommand(vcpilot::MonitorController& controller) : m_controller(controller) {}

void VcpCommand::configure(CLI::App& app) {
    auto* command = app.add_subcommand("vcp", "Raw VCP read/write command");

    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();

    command->add_option("code", m_code, "VCP code")->required();

    command->add_option("value", m_value, "VCP value");

    command->callback([this]() { execute(); });
}

void VcpCommand::execute() {
    const auto monitors = m_controller.getMonitorInfos();

    if (!monitors) {
        std::cerr << "Failed to list monitors: " << monitors.error().message << '\n';
        return;
    }

    if (m_monitorIndex >= monitors->size()) {
        std::cerr << "Invalid monitor index\n";
        return;
    }

    const auto code = parseNumber(m_code);

    if (!code || *code > 0xFF) {
        std::cerr << "Invalid VCP code: " << m_code << " (expected 0x00-0xFF)\n";
        return;
    }

    const auto& monitor = monitors->at(m_monitorIndex);
    const auto vcpCode = static_cast<std::uint8_t>(*code);

    if (m_value.has_value()) {
        const auto value = parseNumber(*m_value);

        if (!value || *value > 0xFFFF) {
            std::cerr << "Invalid VCP value: " << *m_value
                      << " (expected 0-65535 or 0x0000-0xFFFF)\n";
            return;
        }

        const auto result =
            m_controller.setVcp(monitor.id, vcpCode, static_cast<std::uint16_t>(*value));

        if (!result) {
            std::cerr << "Failed to set VCP value: " << result.error().message << '\n';
            return;
        }

        std::cout << "VCP value set successfully\n";
        return;
    }

    const auto result = m_controller.getVcp(monitor.id, vcpCode);

    if (!result) {
        std::cerr << "Failed to get VCP value: " << result.error().message << '\n';
        return;
    }

    std::cout << "VCP 0x" << std::hex << static_cast<unsigned int>(vcpCode) << std::dec << ": "
              << result->current << " / " << result->maximum << '\n';
}

} // namespace vcpilot_cli