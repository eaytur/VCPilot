#include "vcpilot_cli/commands/brightness_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>

namespace vcpilot_cli {

BrightnessCommand::BrightnessCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void BrightnessCommand::configure(CLI::App& app) {
    auto* command = app.add_subcommand("brightness", "Get or set monitor brightness");

    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();

    command->add_option("value", m_value, "Brightness value")->check(CLI::Range(0, 100));

    command->callback([this]() { execute(); });
}

void BrightnessCommand::execute() {
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

    if (m_value.has_value()) {
        const auto result = m_controller.setBrightness(monitor.id, *m_value);

        if (!result) {
            std::cerr << "Failed to set brightness: " << result.error().message << '\n';
        }

        return;
    }

    const auto result = m_controller.getBrightness(monitor.id);

    if (!result) {
        std::cerr << "Failed to get brightness: " << result.error().message << '\n';
        return;
    }

    std::cout << "Brightness: " << result->current << " / " << result->maximum << '\n';
}

} // namespace vcpilot_cli