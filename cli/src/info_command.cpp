#include "vcpilot_cli/commands/info_command.hpp"

#include "vcpilot/mccs_catalog.hpp"
#include "vcpilot/monitor_controller.hpp"

#include <CLI/CLI.hpp>

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

namespace vcpilot_cli {

namespace {

void showSpinner(std::atomic_bool& running) {
    constexpr char frames[] = {'|', '/', '-', '\\'};
    std::size_t index = 0;

    while (running.load()) {
        std::cout << "\rReading capabilities... " << frames[index++ % 4] << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "\r                              \r" << std::flush;
}

} // namespace

InfoCommand::InfoCommand(vcpilot::MonitorController& controller) : m_controller(controller) {}

void InfoCommand::configure(CLI::App& app) {
    auto* command =
        app.add_subcommand("info", "Show monitor capabilities and supported VCP features");

    command->add_option("-m,--monitor", m_monitorIndex, "Monitor index")->required();

    command->callback([this]() { execute(); });
}

void InfoCommand::execute() {
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

    std::cout << "Monitor [" << m_monitorIndex << "]\n";
    std::cout << "ID: " << monitor.id << '\n';

    std::atomic_bool running{true};

    std::thread spinner([&running]() { showSpinner(running); });

    const auto capabilities = m_controller.getCapabilities(monitor.id);

    running.store(false);
    spinner.join();

    if (!capabilities) {
        std::cerr << "Failed to read monitor capabilities: " << capabilities.error().message
                  << '\n';
        return;
    }

    if (!capabilities->mccsVersion.empty()) {
        std::cout << "MCCS version: " << capabilities->mccsVersion << '\n';
    } else {
        std::cout << "MCCS version: unknown\n";
    }

    std::cout << "VCP features: " << capabilities->vcpFeatures.size() << "\n\n";

    for (const auto& feature : capabilities->vcpFeatures) {
        const auto* descriptor = vcpilot::findVcpFeatureDescriptor(feature.code);

        std::cout << "0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(feature.code) << std::dec << std::setfill(' ');

        if (descriptor) {
            std::cout << "  " << descriptor->name << "  [" << vcpilot::toString(descriptor->access)
                      << ", " << vcpilot::toString(descriptor->type) << "]";
        } else {
            std::cout << "  Unknown";
        }

        std::cout << '\n';

        if (!feature.values.empty()) {
            std::cout << "      Values: ";

            for (std::size_t i = 0; i < feature.values.size(); ++i) {
                const auto value = feature.values[i];

                const auto* valueDescriptor = vcpilot::findVcpValueDescriptor(feature.code, value);

                if (i > 0) {
                    std::cout << ", ";
                }

                std::cout << "0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
                          << value << std::dec << std::setfill(' ');

                if (valueDescriptor) {
                    std::cout << " (" << valueDescriptor->name << ")";
                }
            }

            std::cout << '\n';
        }
    }
}

} // namespace vcpilot_cli