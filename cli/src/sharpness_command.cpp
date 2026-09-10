#include "vcpilot_cli/commands/sharpness_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>

namespace vcpilot_cli {

SharpnessCommand::SharpnessCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void SharpnessCommand::configure(CLI::App& app) {
    auto* command =
        app.add_subcommand("sharpness", "Get or set monitor sharpness");

    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option("value", m_value, "Sharpness value");

    command->callback([this]() { execute(); });
}

void SharpnessCommand::execute() {
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
        const auto result = m_controller.setSharpness(monitor.id, *m_value);

        if (!result) {
            std::cerr << "Failed to set sharpness: "
                      << result.error().message << '\n';
        }

        return;
    }

    const auto result = m_controller.getSharpness(monitor.id);

    if (!result) {
        std::cerr << "Failed to get sharpness: "
                  << result.error().message << '\n';
        return;
    }

    std::cout << "Sharpness: "
              << result->current
              << " / "
              << result->maximum
              << '\n';
}

} // namespace vcpilot_cli
