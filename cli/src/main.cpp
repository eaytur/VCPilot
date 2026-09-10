#include "vcpilot/monitor_controller.hpp"

#include "vcpilot_cli/command_registry.hpp"
#include "vcpilot_cli/commands/blue_black_level_command.hpp"
#include "vcpilot_cli/commands/blue_gain_command.hpp"
#include "vcpilot_cli/commands/brightness_command.hpp"
#include "vcpilot_cli/commands/color_preset_command.hpp"
#include "vcpilot_cli/commands/contrast_command.hpp"
#include "vcpilot_cli/commands/gamma_command.hpp"
#include "vcpilot_cli/commands/green_black_level_command.hpp"
#include "vcpilot_cli/commands/green_gain_command.hpp"
#include "vcpilot_cli/commands/info_command.hpp"
#include "vcpilot_cli/commands/input_command.hpp"
#include "vcpilot_cli/commands/list_command.hpp"
#include "vcpilot_cli/commands/mute_command.hpp"
#include "vcpilot_cli/commands/power_command.hpp"
#include "vcpilot_cli/commands/red_black_level_command.hpp"
#include "vcpilot_cli/commands/red_gain_command.hpp"
#include "vcpilot_cli/commands/saturation_command.hpp"
#include "vcpilot_cli/commands/sharpness_command.hpp"
#include "vcpilot_cli/commands/vcp_command.hpp"
#include "vcpilot_cli/commands/volume_command.hpp"

#include <CLI/CLI.hpp>

#include <memory>

int main(int argc, char** argv) {
    CLI::App app{"VCPilot - monitor control from the command line"};

    vcpilot::MonitorController controller;
    vcpilot_cli::CommandRegistry registry;

    registry.add(std::make_unique<vcpilot_cli::ListCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::BrightnessCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::ContrastCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::VolumeCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::SharpnessCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::SaturationCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::GammaCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::RedGainCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::GreenGainCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::BlueGainCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::RedBlackLevelCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::GreenBlackLevelCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::BlueBlackLevelCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::InputCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::MuteCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::PowerCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::ColorPresetCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::VcpCommand>(controller));
    registry.add(std::make_unique<vcpilot_cli::InfoCommand>(controller));

    registry.configureAll(app);

    CLI11_PARSE(app, argc, argv);

    return 0;
}