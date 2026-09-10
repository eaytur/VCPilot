#pragma once

#include "vcpilot_cli/command.hpp"

namespace vcpilot {
class MonitorController;
}

namespace vcpilot_cli {

class ListCommand final : public ICommand {
  public:
    explicit ListCommand(vcpilot::MonitorController& controller);

    void configure(CLI::App& app) override;

  private:
    void execute();

    vcpilot::MonitorController& m_controller;
};

} // namespace vcpilot_cli