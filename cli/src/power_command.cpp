#include "vcpilot_cli/commands/power_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>
#include <optional>
#include <string_view>

namespace {

std::optional<vcpilot::PowerMode> parsePowerMode(std::string_view value) {
    if (value == "on") return vcpilot::PowerMode::On;
    if (value == "standby") return vcpilot::PowerMode::Standby;
    if (value == "suspend") return vcpilot::PowerMode::Suspend;
    if (value == "off") return vcpilot::PowerMode::Off;
    return std::nullopt;
}

std::string_view toString(vcpilot::PowerMode mode) {
    switch (mode) {
    case vcpilot::PowerMode::On: return "on";
    case vcpilot::PowerMode::Standby: return "standby";
    case vcpilot::PowerMode::Suspend: return "suspend";
    case vcpilot::PowerMode::Off: return "off";
    }
    return "unknown";
}

} // namespace

namespace vcpilot_cli {

PowerCommand::PowerCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void PowerCommand::configure(CLI::App& app) {
    auto* command = app.add_subcommand("power", "Get or set monitor power mode");
    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option("mode", m_mode, "Power mode: on, standby, suspend, off");
    command->callback([this]() { execute(); });
}

void PowerCommand::execute() {
    const auto monitors = m_controller.getMonitorInfos();

    if (!monitors) {
        std::cerr << "Failed to list monitors: " << monitors.error().message << '\n';
        return;
    }

    if (m_monitorIndex >= monitors->size()) {
        std::cerr << "Invalid monitor index\n";
        return;
    }

    const auto& monitor = monitors->at(m_monitorIndex);

    if (m_mode.has_value()) {
        const auto mode = parsePowerMode(*m_mode);
        if (!mode) {
            std::cerr << "Invalid power mode: " << *m_mode << '\n';
            return;
        }

        const auto result = m_controller.setPowerMode(monitor.id, *mode);
        if (!result) {
            std::cerr << "Failed to set power mode: " << result.error().message << '\n';
        }
        return;
    }

    const auto result = m_controller.getPowerMode(monitor.id);
    if (!result) {
        std::cerr << "Failed to get power mode: " << result.error().message << '\n';
        return;
    }

    std::cout << "Power: " << toString(*result) << '\n';
}

} // namespace vcpilot_cli
