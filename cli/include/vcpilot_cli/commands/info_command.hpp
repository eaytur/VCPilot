#pragma once

#include "vcpilot_cli/command.hpp"

#include <cstddef>

namespace vcpilot {
class MonitorController;
}

namespace vcpilot_cli {

class InfoCommand final : public ICommand {
  public:
    explicit InfoCommand(vcpilot::MonitorController& controller);

    void configure(CLI::App& app) override;

  private:
    void execute();

    vcpilot::MonitorController& m_controller;
    std::size_t m_monitorIndex{};
};

} // namespace vcpilot_cli