#pragma once

namespace CLI {
class App;
}

namespace vcpilot_cli {

class ICommand {
  public:
    virtual ~ICommand() = default;

    virtual void configure(CLI::App& app) = 0;
};

} // namespace vcpilot_cli