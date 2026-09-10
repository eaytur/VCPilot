#pragma once

#include "vcpilot_cli/command.hpp"

#include <cstddef>
#include <optional>
#include <string>

namespace vcpilot {
class MonitorController;
}

namespace vcpilot_cli {

class ColorPresetCommand final : public ICommand {
  public:
    explicit ColorPresetCommand(vcpilot::MonitorController& controller);

    void configure(CLI::App& app) override;

  private:
    void execute();

    vcpilot::MonitorController& m_controller;
    std::size_t m_monitorIndex{};
    std::optional<std::string> m_preset;
};

} // namespace vcpilot_cli
