#include "vcpilot/monitor_capabilities.hpp"

#include <cctype>
#include <charconv>
#include <optional>
#include <string_view>
#include <utility>

namespace vcpilot {

namespace {

std::optional<std::uint16_t> parseHexValue(std::string_view token) {

    std::uint16_t value = 0;

    const auto result = std::from_chars(token.data(), token.data() + token.size(), value, 16);

    if (result.ec != std::errc{} || result.ptr != token.data() + token.size()) {
        return std::nullopt;
    }

    return value;
}

std::optional<std::string_view> findParenthesizedValue(std::string_view text,
                                                       std::string_view key) {

    const std::string pattern = std::string(key) + "(";

    const auto start = text.find(pattern);

    if (start == std::string_view::npos) {
        return std::nullopt;
    }

    const std::size_t contentStart = start + pattern.size();

    int depth = 1;

    for (std::size_t i = contentStart; i < text.size(); ++i) {

        if (text[i] == '(') {
            ++depth;
        } else if (text[i] == ')') {
            --depth;

            if (depth == 0) {
                return text.substr(contentStart, i - contentStart);
            }
        }
    }

    return std::nullopt;
}

} // namespace

MonitorCapabilities parseCapabilitiesString(const std::string& capabilities) {

    MonitorCapabilities result;

    const std::string_view text{capabilities};

    // ---------------------------------------------------------
    // Parse MCCS version
    // ---------------------------------------------------------

    if (auto version = findParenthesizedValue(text, "mccs_ver")) {

        result.mccsVersion = std::string(*version);
    }

    // ---------------------------------------------------------
    // Parse VCP capabilities
    // ---------------------------------------------------------

    const auto vcpSection = findParenthesizedValue(text, "vcp");

    if (!vcpSection) {
        return result;
    }

    std::size_t pos = 0;

    while (pos < vcpSection->size()) {

        // Skip whitespace.
        while (pos < vcpSection->size() &&
               std::isspace(static_cast<unsigned char>((*vcpSection)[pos]))) {
            ++pos;
        }

        if (pos >= vcpSection->size()) {
            break;
        }

        // Read VCP code.
        const std::size_t codeStart = pos;

        while (pos < vcpSection->size() &&
               std::isxdigit(static_cast<unsigned char>((*vcpSection)[pos]))) {
            ++pos;
        }

        if (codeStart == pos) {
            ++pos;
            continue;
        }

        const auto codeToken = vcpSection->substr(codeStart, pos - codeStart);

        const auto parsedCode = parseHexValue(codeToken);

        if (!parsedCode || *parsedCode > 0xFF) {

            while (pos < vcpSection->size() &&
                   std::isspace(static_cast<unsigned char>((*vcpSection)[pos]))) {
                ++pos;
            }

            if (pos < vcpSection->size() && (*vcpSection)[pos] == '(') {

                int depth = 1;
                ++pos;

                while (pos < vcpSection->size() && depth > 0) {

                    if ((*vcpSection)[pos] == '(') {
                        ++depth;
                    } else if ((*vcpSection)[pos] == ')') {
                        --depth;
                    }

                    ++pos;
                }
            }

            continue;
        }
        VcpCapability capability{
            .code = static_cast<std::uint8_t>(*parsedCode),
            .values = {},
        };

        // Skip whitespace between code and optional "(".
        while (pos < vcpSection->size() &&
               std::isspace(static_cast<unsigned char>((*vcpSection)[pos]))) {
            ++pos;
        }

        // -----------------------------------------------------
        // Optional value list:
        //
        // 60(0F 11)
        // 14(04 05 06 08 0B)
        // -----------------------------------------------------

        if (pos < vcpSection->size() && (*vcpSection)[pos] == '(') {

            ++pos;

            while (pos < vcpSection->size() && (*vcpSection)[pos] != ')') {

                while (pos < vcpSection->size() &&
                       std::isspace(static_cast<unsigned char>((*vcpSection)[pos]))) {
                    ++pos;
                }

                if (pos >= vcpSection->size() || (*vcpSection)[pos] == ')') {
                    break;
                }

                const std::size_t valueStart = pos;

                while (pos < vcpSection->size() &&
                       std::isxdigit(static_cast<unsigned char>((*vcpSection)[pos]))) {
                    ++pos;
                }

                if (valueStart == pos) {
                    ++pos;
                    continue;
                }

                const auto valueToken = vcpSection->substr(valueStart, pos - valueStart);

                if (auto value = parseHexValue(valueToken)) {

                    capability.values.push_back(*value);
                }
            }

            if (pos < vcpSection->size() && (*vcpSection)[pos] == ')') {
                ++pos;
            }
        }

        result.vcpFeatures.push_back(std::move(capability));
    }

    return result;
}

} // namespace vcpilot