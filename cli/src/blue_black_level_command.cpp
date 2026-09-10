#include "vcpilot_cli/commands/blue_black_level_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>

namespace vcpilot_cli {

BlueBlackLevelCommand::BlueBlackLevelCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void BlueBlackLevelCommand::configure(CLI::App& app) {
    auto* command =
        app.add_subcommand("blue-black-level", "Get or set monitor blue black level");

    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option("value", m_value, "Blue black level value");

    command->callback([this]() { execute(); });
}

void BlueBlackLevelCommand::execute() {
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
        const auto result = m_controller.setBlueBlackLevel(monitor.id, *m_value);

        if (!result) {
            std::cerr << "Failed to set blue black level: "
                      << result.error().message << '\n';
        }

        return;
    }

    const auto result = m_controller.getBlueBlackLevel(monitor.id);

    if (!result) {
        std::cerr << "Failed to get blue black level: "
                  << result.error().message << '\n';
        return;
    }

    std::cout << "Blue black level: "
              << result->current
              << " / "
              << result->maximum
              << '\n';
}

} // namespace vcpilot_cli
