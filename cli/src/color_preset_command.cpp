#include "vcpilot_cli/commands/color_preset_command.hpp"

#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <iostream>
#include <optional>
#include <string_view>

namespace {

std::optional<vcpilot::ColorPreset> parseColorPreset(std::string_view value) {
    if (value == "srgb") return vcpilot::ColorPreset::SRgb;
    if (value == "native") return vcpilot::ColorPreset::DisplayNative;
    if (value == "4000k") return vcpilot::ColorPreset::K4000;
    if (value == "5000k") return vcpilot::ColorPreset::K5000;
    if (value == "6500k") return vcpilot::ColorPreset::K6500;
    if (value == "7500k") return vcpilot::ColorPreset::K7500;
    if (value == "8200k") return vcpilot::ColorPreset::K8200;
    if (value == "9300k") return vcpilot::ColorPreset::K9300;
    if (value == "10000k") return vcpilot::ColorPreset::K10000;
    if (value == "11500k") return vcpilot::ColorPreset::K11500;
    if (value == "user1") return vcpilot::ColorPreset::User1;
    if (value == "user2") return vcpilot::ColorPreset::User2;
    if (value == "user3") return vcpilot::ColorPreset::User3;
    return std::nullopt;
}

std::string_view toString(vcpilot::ColorPreset preset) {
    switch (preset) {
    case vcpilot::ColorPreset::SRgb: return "srgb";
    case vcpilot::ColorPreset::DisplayNative: return "native";
    case vcpilot::ColorPreset::K4000: return "4000k";
    case vcpilot::ColorPreset::K5000: return "5000k";
    case vcpilot::ColorPreset::K6500: return "6500k";
    case vcpilot::ColorPreset::K7500: return "7500k";
    case vcpilot::ColorPreset::K8200: return "8200k";
    case vcpilot::ColorPreset::K9300: return "9300k";
    case vcpilot::ColorPreset::K10000: return "10000k";
    case vcpilot::ColorPreset::K11500: return "11500k";
    case vcpilot::ColorPreset::User1: return "user1";
    case vcpilot::ColorPreset::User2: return "user2";
    case vcpilot::ColorPreset::User3: return "user3";
    }
    return "unknown";
}

} // namespace

namespace vcpilot_cli {

ColorPresetCommand::ColorPresetCommand(vcpilot::MonitorController& controller)
    : m_controller(controller) {}

void ColorPresetCommand::configure(CLI::App& app) {
    auto* command = app.add_subcommand("color-preset", "Get or set monitor color preset");
    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();
    command->add_option(
        "preset", m_preset,
        "Preset: srgb, native, 4000k, 5000k, 6500k, 7500k, 8200k, 9300k, "
        "10000k, 11500k, user1, user2, user3");
    command->callback([this]() { execute(); });
}

void ColorPresetCommand::execute() {
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

    if (m_preset.has_value()) {
        const auto preset = parseColorPreset(*m_preset);
        if (!preset) {
            std::cerr << "Invalid color preset: " << *m_preset << '\n';
            return;
        }

        const auto result = m_controller.setColorPreset(monitor.id, *preset);
        if (!result) {
            std::cerr << "Failed to set color preset: " << result.error().message << '\n';
        }
        return;
    }

    const auto result = m_controller.getColorPreset(monitor.id);
    if (!result) {
        std::cerr << "Failed to get color preset: " << result.error().message << '\n';
        return;
    }

    std::cout << "Color preset: " << toString(*result) << '\n';
}

} // namespace vcpilot_cli
