#include <spdlog/spdlog.h>
#include <vcpilot/Logger.hpp>

namespace vcpilot::log {

namespace {
spdlog::level::level_enum toSpdlogLevel(Level level) {
    switch (level) {
    case Level::Trace:
        return spdlog::level::trace;
    case Level::Debug:
        return spdlog::level::debug;
    case Level::Info:
        return spdlog::level::info;
    case Level::Warning:
        return spdlog::level::warn;
    case Level::Error:
        return spdlog::level::err;
    case Level::Critical:
        return spdlog::level::critical;
    case Level::Off:
        return spdlog::level::off;
    }

    return spdlog::level::trace;
}

} // namespace

void initialize() {
    spdlog::set_level(spdlog::level::trace); // TODO: set build type
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^[%l]%$ %v");
}

void setLevel(Level level) {
    spdlog::set_level(toSpdlogLevel(level));
}

void logMessage(Level level, std::string_view message) {
    spdlog::log(toSpdlogLevel(level), message);
}

} // namespace vcpilot::log