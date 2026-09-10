#include "vcpilot_cli/commands/saturation_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>

namespace vcpilot_cli {

SaturationCommand::SaturationCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void SaturationCommand::configure(CLI::App& app) {
    auto* command =
        app.add_subcommand("saturation", "Get or set monitor saturation");

    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option("value", m_value, "Saturation value");

    command->callback([this]() { execute(); });
}

void SaturationCommand::execute() {
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
        const auto result = m_controller.setSaturation(monitor.id, *m_value);

        if (!result) {
            std::cerr << "Failed to set saturation: "
                      << result.error().message << '\n';
        }

        return;
    }

    const auto result = m_controller.getSaturation(monitor.id);

    if (!result) {
        std::cerr << "Failed to get saturation: "
                  << result.error().message << '\n';
        return;
    }

    std::cout << "Saturation: "
              << result->current
              << " / "
              << result->maximum
              << '\n';
}

} // namespace vcpilot_cli
