#include "vcpilot_cli/commands/volume_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>

namespace vcpilot_cli {

VolumeCommand::VolumeCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void VolumeCommand::configure(CLI::App& app) {
    auto* command =
        app.add_subcommand("volume", "Get or set monitor volume");

    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option("value", m_value, "Volume value");

    command->callback([this]() { execute(); });
}

void VolumeCommand::execute() {
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
        const auto result = m_controller.setVolume(monitor.id, *m_value);

        if (!result) {
            std::cerr << "Failed to set volume: "
                      << result.error().message << '\n';
        }

        return;
    }

    const auto result = m_controller.getVolume(monitor.id);

    if (!result) {
        std::cerr << "Failed to get volume: "
                  << result.error().message << '\n';
        return;
    }

    std::cout << "Volume: "
              << result->current
              << " / "
              << result->maximum
              << '\n';
}

} // namespace vcpilot_cli
