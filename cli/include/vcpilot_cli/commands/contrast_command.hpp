#pragma once

#include "vcpilot_cli/command.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace vcpilot {
class MonitorController;
}

namespace vcpilot_cli {

class ContrastCommand final : public ICommand {
  public:
    explicit ContrastCommand(vcpilot::MonitorController& controller);

    void configure(CLI::App& app) override;

  private:
    void execute();

    vcpilot::MonitorController& m_controller;
    std::size_t m_monitorIndex{};
    std::optional<std::uint16_t> m_value;
};

} // namespace vcpilot_cli
