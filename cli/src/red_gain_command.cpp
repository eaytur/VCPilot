#include "vcpilot_cli/commands/red_gain_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>

namespace vcpilot_cli {

RedGainCommand::RedGainCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void RedGainCommand::configure(CLI::App& app) {
    auto* command =
        app.add_subcommand("red-gain", "Get or set monitor red gain");

    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option("value", m_value, "Red gain value");

    command->callback([this]() { execute(); });
}

void RedGainCommand::execute() {
    const auto monitors = m_controller.getMonitorInfos();

    if (!monitors) {
        std::cerr << "Failed to list monitors: "
                  << monitors.error().message << '\n';
        return;
    }

    if (m_monitorIndex >= monitors->size()) {
        std::cerr << "Invalid monitor index\n";
        return;
    }

    const auto& monitor = monitors->at(m_monitorIndex);

    if (m_value.has_value()) {
        const auto result = m_controller.setRedGain(monitor.id, *m_value);

        if (!result) {
            std::cerr << "Failed to set red gain: "
                      << result.error().message << '\n';
        }

        return;
    }

    const auto result = m_controller.getRedGain(monitor.id);

    if (!result) {
        std::cerr << "Failed to get red gain: "
                  << result.error().message << '\n';
        return;
    }

    std::cout << "Red gain: "
              << result->current
              << " / "
              << result->maximum
              << '\n';
}

} // namespace vcpilot_cli
