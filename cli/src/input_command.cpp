#include "vcpilot_cli/commands/input_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>
#include <optional>
#include <string_view>

namespace {

std::optional<vcpilot::InputSource> parseInputSource(std::string_view value) {
    if (value == "vga1")
        return vcpilot::InputSource::Vga1;
    if (value == "vga2")
        return vcpilot::InputSource::Vga2;
    if (value == "dvi1")
        return vcpilot::InputSource::Dvi1;
    if (value == "dvi2")
        return vcpilot::InputSource::Dvi2;
    if (value == "dp1")
        return vcpilot::InputSource::DisplayPort1;
    if (value == "dp2")
        return vcpilot::InputSource::DisplayPort2;
    if (value == "hdmi1")
        return vcpilot::InputSource::Hdmi1;
    if (value == "hdmi2")
        return vcpilot::InputSource::Hdmi2;
    return std::nullopt;
}

} // namespace

namespace vcpilot_cli {

InputCommand::InputCommand(vcpilot::MonitorController& controller) : m_controller(controller) {}

void InputCommand::configure(CLI::App& app) {
    auto* command = app.add_subcommand("input", "Get or set monitor input source");
    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option("source", m_source,
                        "Input source: vga1, vga2, dvi1, dvi2, dp1, dp2, hdmi1, hdmi2");
    command->callback([this]() { execute(); });
}

void InputCommand::execute() {
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

    if (m_source.has_value()) {
        const auto source = parseInputSource(*m_source);
        if (!source) {
            std::cerr << "Invalid input source: " << *m_source << '\n';
            return;
        }

        const auto result = m_controller.setInputSource(monitor.id, *source);
        if (!result) {
            std::cerr << "Failed to set input source: " << result.error().message << '\n';
        }
        return;
    }

    const auto result = m_controller.getInputSource(monitor.id);
    if (!result) {
        std::cerr << "Failed to get input source: " << result.error().message << '\n';
        return;
    }

    std::cout << "Input: " << toString(*result) << '\n';
}

} // namespace vcpilot_cli
