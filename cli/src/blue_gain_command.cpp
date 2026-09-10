#include "vcpilot_cli/commands/blue_gain_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>

namespace vcpilot_cli {

BlueGainCommand::BlueGainCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void BlueGainCommand::configure(CLI::App& app) {
    auto* command =
        app.add_subcommand("blue-gain", "Get or set monitor blue gain");

    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option("value", m_value, "Blue gain value");

    command->callback([this]() { execute(); });
}

void BlueGainCommand::execute() {
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
        const auto result = m_controller.setBlueGain(monitor.id, *m_value);

        if (!result) {
            std::cerr << "Failed to set blue gain: "
                      << result.error().message << '\n';
        }

        return;
    }

    const auto result = m_controller.getBlueGain(monitor.id);

    if (!result) {
        std::cerr << "Failed to get blue gain: "
                  << result.error().message << '\n';
        return;
    }

    std::cout << "Blue gain: "
              << result->current
              << " / "
              << result->maximum
              << '\n';
}

} // namespace vcpilot_cli
