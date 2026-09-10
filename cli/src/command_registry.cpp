#include "vcpilot_cli/command_registry.hpp"
#include "vcpilot_cli/command.hpp"

#include <utility>

namespace vcpilot_cli {

void CommandRegistry::add(std::unique_ptr<ICommand> command) {
    m_commands.push_back(std::move(command));
}

void CommandRegistry::configureAll(CLI::App& app) {
    for (const auto& command : m_commands) {
        command->configure(app);
    }
}

} // namespace vcpilot_cli
