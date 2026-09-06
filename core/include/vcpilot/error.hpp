#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <string>

namespace vcpilot{

enum class ErrorCode {
    Unknown,
};

struct Error {
    ErrorCode code;
    std::string message;
    std::optional<std::uint32_t> nativeCode;
};

template <typename T>
using Result = std::expected<T, Error>;

} // namespace vcpilot
