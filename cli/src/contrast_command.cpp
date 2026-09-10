#include "vcpilot_cli/commands/contrast_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>

namespace vcpilot_cli {

ContrastCommand::ContrastCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void ContrastCommand::configure(CLI::App& app) {
    auto* command =
        app.add_subcommand("contrast", "Get or set monitor contrast");

    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option("value", m_value, "Contrast value");

    command->callback([this]() { execute(); });
}

void ContrastCommand::execute() {
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
        const auto result = m_controller.setContrast(monitor.id, *m_value);

        if (!result) {
            std::cerr << "Failed to set contrast: "
                      << result.error().message << '\n';
        }

        return;
    }

    const auto result = m_controller.getContrast(monitor.id);

    if (!result) {
        std::cerr << "Failed to get contrast: "
                  << result.error().message << '\n';
        return;
    }

    std::cout << "Contrast: "
              << result->current
              << " / "
              << result->maximum
              << '\n';
}

} // namespace vcpilot_cli
