#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <string>

namespace vcpilot {

enum class ErrorCode {
    Unknown,
    EnumerationFailed,
    MonitorNotFound,
    VcpReadFailed,
    VcpWriteFailed,
    UnsupportedInputSource,
};

constexpr std::string_view toString(ErrorCode code) {
    switch (code) {
    case ErrorCode::Unknown:
        return "Unknown";
    case ErrorCode::EnumerationFailed:
        return "EnumerationFailed";
    case ErrorCode::MonitorNotFound:
        return "MonitorNotFound";
    case ErrorCode::VcpReadFailed:
        return "VcpReadFailed";
    case ErrorCode::VcpWriteFailed:
        return "VcpWriteFailed";
    case ErrorCode::UnsupportedInputSource:
        return "UnsupportedInputSource";
    }

    return "Unknown";
}

struct Error {
    ErrorCode code;
    std::string message;
    std::optional<std::uint32_t> nativeCode;
};

template <typename T> using Result = std::expected<T, Error>;

} // namespace vcpilot
