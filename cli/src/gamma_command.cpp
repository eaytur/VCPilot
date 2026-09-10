#include "vcpilot_cli/commands/gamma_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>

namespace vcpilot_cli {

GammaCommand::GammaCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void GammaCommand::configure(CLI::App& app) {
    auto* command =
        app.add_subcommand("gamma", "Get or set monitor gamma");

    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option("value", m_value, "Gamma value");

    command->callback([this]() { execute(); });
}

void GammaCommand::execute() {
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
        const auto result = m_controller.setGamma(monitor.id, *m_value);

        if (!result) {
            std::cerr << "Failed to set gamma: "
                      << result.error().message << '\n';
        }

        return;
    }

    const auto result = m_controller.getGamma(monitor.id);

    if (!result) {
        std::cerr << "Failed to get gamma: "
                  << result.error().message << '\n';
        return;
    }

    std::cout << "Gamma: "
              << result->current
              << " / "
              << result->maximum
              << '\n';
}

} // namespace vcpilot_cli
