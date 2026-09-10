#include "vcpilot_cli/commands/mute_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>
#include <optional>
#include <string_view>

namespace {

std::optional<bool> parseMuteState(std::string_view value) {
    if (value == "on" || value == "true" || value == "1") return true;
    if (value == "off" || value == "false" || value == "0") return false;
    return std::nullopt;
}

} // namespace

namespace vcpilot_cli {

MuteCommand::MuteCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void MuteCommand::configure(CLI::App& app) {
    auto* command = app.add_subcommand("mute", "Get or set monitor mute state");
    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option("state", m_state, "Mute state: on or off");
    command->callback([this]() { execute(); });
}

void MuteCommand::execute() {
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

    if (m_state.has_value()) {
        const auto muted = parseMuteState(*m_state);
        if (!muted.has_value()) {
            std::cerr << "Invalid mute state: " << *m_state << '\n';
            return;
        }

        const auto result = m_controller.setMute(monitor.id, *muted);
        if (!result) {
            std::cerr << "Failed to set mute state: " << result.error().message << '\n';
        }
        return;
    }

    const auto result = m_controller.getMute(monitor.id);
    if (!result) {
        std::cerr << "Failed to get mute state: " << result.error().message << '\n';
        return;
    }

    std::cout << "Mute: " << (*result ? "on" : "off") << '\n';
}

} // namespace vcpilot_cli
