#include "windows_ddc_backend.hpp"

#include "vcpilot/error.hpp"
#include "vcpilot/logger.hpp"

// clang-format off
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
// clang-format on

#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace vcpilot {

namespace {

// --- Windows device-id / registry constants ---------------------------

constexpr std::size_t kMonitorIdPrefixLength = 8;
constexpr DWORD kSetupApiStringBufferSize = 512;

// --- EDID (VESA E-EDID 1.4) layout constants --------------------------

constexpr std::size_t kEdidBaseBlockSize = 128;

constexpr std::size_t kEdidManufacturerIdOffset = 8;
constexpr std::size_t kEdidProductCodeOffset = 10;
constexpr std::size_t kEdidSerialNumberOffset = 12;

constexpr std::size_t kEdidDescriptorBlockOffset = 54;
constexpr std::size_t kEdidDescriptorBlockSize = 18;
constexpr int kEdidDescriptorBlockCount = 4;

constexpr std::size_t kEdidDescriptorTextOffset = 5;
constexpr std::size_t kEdidDescriptorTextMaxLength = 13;

constexpr std::uint8_t kEdidDescriptorTypeMonitorName = 0xFC;
constexpr std::uint8_t kEdidDescriptorTypeSerialNumber = 0xFF;

constexpr int kEdidManufacturerLetterBits = 5;
constexpr std::uint16_t kEdidManufacturerLetterMask = 0x1F;

// ---------------------------------------------------------------------
// Extracts "SAM76E3" from:
// "MONITOR\SAM76E3\{...}\0005"
// ---------------------------------------------------------------------

std::wstring extractHardwareId(const wchar_t* deviceId) {
    std::wstring id(deviceId);

    auto begin = id.find(L"MONITOR\\");
    if (begin == std::wstring::npos) {
        return {};
    }

    begin += kMonitorIdPrefixLength;

    auto end = id.find(L'\\', begin);

    return id.substr(begin, end == std::wstring::npos ? std::wstring::npos : end - begin);
}

// ---------------------------------------------------------------------
// Finds the Windows monitor device and reads its EDID from registry.
// ---------------------------------------------------------------------

std::optional<std::vector<std::uint8_t>> readEdid(const std::wstring& hwId) {
    HDEVINFO devSet = SetupDiGetClassDevsW(&GUID_DEVCLASS_MONITOR, nullptr, nullptr, DIGCF_PRESENT);

    if (devSet == INVALID_HANDLE_VALUE) {
        return std::nullopt;
    }

    std::optional<std::vector<std::uint8_t>> edidOut;

    SP_DEVINFO_DATA devInfo{};
    devInfo.cbSize = sizeof(devInfo);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(devSet, i, &devInfo); ++i) {
        wchar_t hwIds[kSetupApiStringBufferSize]{};

        if (!SetupDiGetDeviceRegistryPropertyW(devSet, &devInfo, SPDRP_HARDWAREID, nullptr,
                                               reinterpret_cast<PBYTE>(hwIds), sizeof(hwIds),
                                               nullptr)) {
            continue;
        }

        if (std::wstring(hwIds).find(hwId) == std::wstring::npos) {
            continue;
        }

        wchar_t instanceId[kSetupApiStringBufferSize]{};

        if (!SetupDiGetDeviceInstanceIdW(devSet, &devInfo, instanceId,
                                         static_cast<DWORD>(std::size(instanceId)), nullptr)) {
            continue;
        }

        std::wstring path = L"SYSTEM\\CurrentControlSet\\Enum\\" + std::wstring(instanceId) +
                            L"\\Device Parameters";

        HKEY key{};

        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ, &key) != ERROR_SUCCESS) {
            continue;
        }

        DWORD type = 0;
        DWORD size = 0;

        if (RegQueryValueExW(key, L"EDID", nullptr, &type, nullptr, &size) == ERROR_SUCCESS &&
            type == REG_BINARY && size > 0) {
            std::vector<std::uint8_t> edid(size);

            if (RegQueryValueExW(key, L"EDID", nullptr, nullptr, edid.data(), &size) ==
                ERROR_SUCCESS) {
                edidOut = std::move(edid);
            }
        }

        RegCloseKey(key);

        if (edidOut) {
            break;
        }
    }

    SetupDiDestroyDeviceInfoList(devSet);

    return edidOut;
}

// ---------------------------------------------------------------------
// Extracts the ASCII text stored inside an EDID descriptor.
// ---------------------------------------------------------------------

std::string parseDescriptorText(const std::uint8_t* descriptorBlock) {
    std::string text(reinterpret_cast<const char*>(descriptorBlock + kEdidDescriptorTextOffset),
                     kEdidDescriptorTextMaxLength);

    auto end = text.find_first_of("\r\n\0", 0, 3);

    if (end != std::string::npos) {
        text.resize(end);
    }

    while (!text.empty() && text.back() == ' ') {
        text.pop_back();
    }

    return text;
}

// ---------------------------------------------------------------------
// Parses EDID fields that are useful to MonitorInfo.
// ---------------------------------------------------------------------

void parseEdidInto(const std::vector<std::uint8_t>& edid, MonitorInfo& info) {
    if (edid.size() < kEdidBaseBlockSize) {
        return;
    }

    // Manufacturer ID is stored as three packed 5-bit letters.
    std::uint16_t manufacturerId =
        (edid[kEdidManufacturerIdOffset] << 8) | edid[kEdidManufacturerIdOffset + 1];

    info.manufacturer = {
        static_cast<char>(
            ((manufacturerId >> (2 * kEdidManufacturerLetterBits)) & kEdidManufacturerLetterMask) +
            'A' - 1),

        static_cast<char>(
            ((manufacturerId >> kEdidManufacturerLetterBits) & kEdidManufacturerLetterMask) + 'A' -
            1),

        static_cast<char>((manufacturerId & kEdidManufacturerLetterMask) + 'A' - 1)};

    // Product code is little-endian.
    std::uint16_t productCode =
        edid[kEdidProductCodeOffset] | (edid[kEdidProductCodeOffset + 1] << 8);

    // Numeric serial number is also little-endian.
    std::uint32_t numericSerial =
        edid[kEdidSerialNumberOffset] | (edid[kEdidSerialNumberOffset + 1] << 8) |
        (edid[kEdidSerialNumberOffset + 2] << 16) | (edid[kEdidSerialNumberOffset + 3] << 24);

    // Search the four EDID descriptor blocks.
    for (int i = 0; i < kEdidDescriptorBlockCount; ++i) {

        const std::uint8_t* block =
            edid.data() + kEdidDescriptorBlockOffset + i * kEdidDescriptorBlockSize;

        // Non-zero first three bytes means this is not
        // a display descriptor.
        if (block[0] || block[1] || block[2]) {
            continue;
        }

        const std::uint8_t descriptorType = block[3];

        if (descriptorType == kEdidDescriptorTypeMonitorName) {
            info.model = parseDescriptorText(block);
        } else if (descriptorType == kEdidDescriptorTypeSerialNumber) {
            info.serial = parseDescriptorText(block);
        }
    }

    // Some monitors don't provide the text serial descriptor.
    if (info.serial.empty() && numericSerial != 0) {
        info.serial = std::to_string(numericSerial);
    }

    // Some monitors don't provide a monitor-name descriptor.
    // Fall back to the EDID product code.
    if (info.model.empty()) {

        std::ostringstream ss;

        ss << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << productCode;

        info.model = ss.str();
    }
}

// ---------------------------------------------------------------------
// Called once by EnumDisplayMonitors for every logical monitor.
// ---------------------------------------------------------------------

BOOL CALLBACK monitorEnumProc(HMONITOR monitorHandle, HDC, LPRECT, LPARAM data) {
    MONITORINFOEXW monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (!GetMonitorInfoW(monitorHandle, reinterpret_cast<LPMONITORINFO>(&monitorInfo))) {
        VCPLOG_WARN("GetMonitorInfoW failed, native error={}", GetLastError());
        return TRUE; // skip this monitor, keep enumerating
    }
    DISPLAY_DEVICEW displayDevice{};
    displayDevice.cb = sizeof(displayDevice);

    if (!EnumDisplayDevicesW(monitorInfo.szDevice, 0, &displayDevice, 0)) {
        return FALSE;
    }

    MonitorInfo info;

    info.isPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;

    info.bounds.x = monitorInfo.rcMonitor.left;

    info.bounds.y = monitorInfo.rcMonitor.top;

    info.bounds.width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;

    info.bounds.height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

    const std::wstring hardwareId = extractHardwareId(displayDevice.DeviceID);

    if (auto edid = readEdid(hardwareId)) {
        parseEdidInto(*edid, info);
    }

    auto* monitors = reinterpret_cast<std::vector<MonitorInfo>*>(data);

    monitors->push_back(std::move(info));

    return TRUE;
}

} // namespace

Result<std::vector<MonitorInfo>> WindowsDdcBackend::listMonitors() {
    std::vector<MonitorInfo> monitors;

    const BOOL result =
        EnumDisplayMonitors(nullptr, nullptr, monitorEnumProc, reinterpret_cast<LPARAM>(&monitors));

    if (!result) {
        return std::unexpected(Error{.code = ErrorCode::EnumerationFailed,
                                     .message = "Failed to enumerate monitors",
                                     .nativeCode = GetLastError()});
    }

    for (const auto& monitor : monitors) {
        VCPLOG_TRACE("Monitor: manufacturer='{}', model='{}', "
                     "serial='{}', primary={}, {}x{}, x={}, y={},",
                     monitor.manufacturer, monitor.model, monitor.serial, monitor.isPrimary,
                     monitor.bounds.width, monitor.bounds.height, monitor.bounds.x,
                     monitor.bounds.y);
    }

    return monitors;
}

} // namespace vcpilot