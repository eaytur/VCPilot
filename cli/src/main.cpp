#include "vcpilot/monitor_controller.hpp"

#include "vcpilot_cli/command_registry.hpp"
#include "vcpilot_cli/commands/brightness_command.hpp"
#include "vcpilot_cli/commands/list_command.hpp"

#include <CLI/CLI.hpp>

#include <memory>

int main(int argc, char** argv) {
    CLI::App app{"VCPilot - monitor control from the command line"};

    vcpilot::MonitorController controller;

    vcpilot_cli::CommandRegistry registry;

    registry.add(std::make_unique<vcpilot_cli::ListCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::BrightnessCommand>(controller));

    registry.configureAll(app);

    CLI11_PARSE(app, argc, argv);

    return 0;
}