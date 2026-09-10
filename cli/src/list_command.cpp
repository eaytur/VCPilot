#include "vcpilot_cli/commands/list_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>

namespace vcpilot_cli {

ListCommand::ListCommand(vcpilot::MonitorController& controller) : m_controller(controller) {}

void ListCommand::configure(CLI::App& app) {
    auto* command = app.add_subcommand("list", "List connected monitors");

    command->callback([this]() { execute(); });
}

void ListCommand::execute() {
    const auto monitors = m_controller.getMonitorInfos();

    if (!monitors) {
        std::cerr << "Failed to list monitors: " << monitors.error().message << '\n';
        return;
    }

    for (std::size_t i = 0; i < monitors->size(); ++i) {
        const auto& monitor = monitors->at(i);

        std::cout << "[" << i << "] " << monitor.manufacturer << " " << monitor.model << '\n';

        std::cout << "    Serial: " << monitor.serial << '\n';

        std::cout << "    ID: " << monitor.id << '\n';

        std::cout << "    Primary: " << (monitor.isPrimary ? "Yes" : "No") << '\n';

        std::cout << '\n';
    }
}

} // namespace vcpilot_cli