#pragma once

#include <fmt/format.h>
#include <utility>
#include <string_view>

namespace vcpilot::log {

enum class Level {
     Trace, 
     Debug, 
     Info, 
     Warning, 
     Error, 
     Critical, 
     Off 
};

void initialize();
void setLevel(Level level);
void logMessage(Level level, std::string_view message);

template<typename... Args>
void logFmt(Level level, fmt::format_string<Args...> format, Args&&... args) {
    logMessage(level, fmt::format(format, std::forward<Args>(args)...));
}

} // namespace vcpilot::log

#define VCPLOG_TRACE(...)    ::vcpilot::log::logFmt(::vcpilot::log::Level::Trace, __VA_ARGS__)
#define VCPLOG_DEBUG(...)    ::vcpilot::log::logFmt(::vcpilot::log::Level::Debug, __VA_ARGS__)
#define VCPLOG_INFO(...)     ::vcpilot::log::logFmt(::vcpilot::log::Level::Info, __VA_ARGS__)
#define VCPLOG_WARN(...)     ::vcpilot::log::logFmt(::vcpilot::log::Level::Warning, __VA_ARGS__)
#define VCPLOG_ERROR(...)    ::vcpilot::log::logFmt(::vcpilot::log::Level::Error, __VA_ARGS__)
#define VCPLOG_CRITICAL(...) ::vcpilot::log::logFmt(::vcpilot::log::Level::Critical, __VA_ARGS__)
