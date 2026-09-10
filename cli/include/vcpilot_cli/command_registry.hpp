#pragma once

#include <memory>
#include <vector>

namespace CLI {
class App;
}

namespace vcpilot_cli {

class ICommand;

class CommandRegistry {
  public:
    void add(std::unique_ptr<ICommand> command);

    void configureAll(CLI::App& app);

  private:
    std::vector<std::unique_ptr<ICommand>> m_commands;
};

} // namespace vcpilot_cli