#include "windows_ddc_backend.hpp"

#include "vcpilot/error.hpp"
#include "vcpilot/logger.hpp"
#include "vcpilot/monitor_capabilities.hpp"

// clang-format off
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
// clang-format on

#include <lowlevelmonitorconfigurationapi.h>

#include <algorithm>
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

    // Some context don't provide the text serial descriptor.
    if (info.serial.empty() && numericSerial != 0) {
        info.serial = std::to_string(numericSerial);
    }

    // Some context don't provide a monitor-name descriptor.
    // Fall back to the EDID product code.
    if (info.model.empty()) {

        std::ostringstream ss;

        ss << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << productCode;

        info.model = ss.str();
    }
}

// ---------------------------------------------------------------------
// Converts wide to utf8 format
// ---------------------------------------------------------------------
std::string WideToUtf8(const wchar_t* wide) {
    if (!wide || *wide == L'\0') {
        return {};
    }

    const int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);

    if (sizeNeeded <= 0) {
        return {};
    }

    std::string result(static_cast<std::size_t>(sizeNeeded), '\0');

    const int convertedSize =
        WideCharToMultiByte(CP_UTF8, 0, wide, -1, result.data(), sizeNeeded, nullptr, nullptr);

    if (convertedSize <= 0) {
        return {};
    }

    result.pop_back();

    return result;
}
} // namespace

WindowsDdcBackend::~WindowsDdcBackend() {
    clearMonitorHandles();
}

BOOL CALLBACK WindowsDdcBackend::monitorEnumProc(HMONITOR monitorHandle, HDC, LPRECT, LPARAM data) {
    MONITORINFOEXW monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (!GetMonitorInfoW(monitorHandle, reinterpret_cast<LPMONITORINFO>(&monitorInfo))) {
        VCPLOG_WARN("GetMonitorInfoW failed, native error={}", GetLastError());
        return TRUE; // skip this monitor, keep enumerating
    }
    DISPLAY_DEVICEW displayDevice{};
    displayDevice.cb = sizeof(displayDevice);

    if (!EnumDisplayDevicesW(monitorInfo.szDevice, 0, &displayDevice, 0)) {
        VCPLOG_WARN("EnumDisplayDevicesW failed, native error={}", GetLastError());
        return TRUE; // skip this monitor, keep enumerating
    }

    MonitorInfo info;

    info.id = WideToUtf8(displayDevice.DeviceID);

    info.isPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;

    info.bounds.x = monitorInfo.rcMonitor.left;

    info.bounds.y = monitorInfo.rcMonitor.top;

    info.bounds.width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;

    info.bounds.height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

    const std::wstring hardwareId = extractHardwareId(displayDevice.DeviceID);

    if (auto edid = readEdid(hardwareId)) {
        parseEdidInto(*edid, info);
    }

    DWORD physicalMonitorCount = 0;

    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(monitorHandle, &physicalMonitorCount)) {
        VCPLOG_WARN("GetNumberOfPhysicalMonitorsFromHMONITOR failed, native error={}",
                    GetLastError());

        return TRUE;
    }

    std::vector<PHYSICAL_MONITOR> physicalMonitors(physicalMonitorCount);

    if (!GetPhysicalMonitorsFromHMONITOR(monitorHandle, physicalMonitorCount,
                                         physicalMonitors.data())) {
        VCPLOG_WARN("GetPhysicalMonitorsFromHMONITOR failed, native error={}", GetLastError());

        return TRUE;
    }

    MonitorHandleEntry handleEntry;
    handleEntry.id = info.id;
    handleEntry.logicalHandle = monitorHandle;
    handleEntry.physicalMonitors = std::move(physicalMonitors);

    auto* context = reinterpret_cast<EnumerationContext*>(data);

    context->handles.push_back(std::move(handleEntry));
    context->monitors.push_back(std::move(info));

    return TRUE;
}

Result<WindowsDdcBackend::EnumerationContext> WindowsDdcBackend::enumerateMonitors() {
    EnumerationContext context;

    const BOOL result =
        EnumDisplayMonitors(nullptr, nullptr, monitorEnumProc, reinterpret_cast<LPARAM>(&context));

    if (!result) {
        const DWORD error = GetLastError();

        destroyMonitorHandles(context.handles);

        return std::unexpected(Error{
            .code = ErrorCode::EnumerationFailed,
            .message = "Failed to enumerate monitors",
            .nativeCode = error,
        });
    }
    return context;
}

Result<std::vector<MonitorInfo>> WindowsDdcBackend::listMonitors() {
    auto enumerationResult = enumerateMonitors();

    if (!enumerationResult) {
        return std::unexpected(enumerationResult.error());
    }

    auto context = std::move(*enumerationResult);

    for (const auto& monitor : context.monitors) {
        VCPLOG_TRACE("Monitor: manufacturer='{}', model='{}', "
                     "serial='{}', primary={}, {}x{}, x={}, y={}",
                     monitor.manufacturer, monitor.model, monitor.serial, monitor.isPrimary,
                     monitor.bounds.width, monitor.bounds.height, monitor.bounds.x,
                     monitor.bounds.y);
    }

    clearMonitorHandles();
    m_monitorHandles = std::move(context.handles);

    return std::move(context.monitors);
}

Result<void> WindowsDdcBackend::refreshMonitorHandles() {
    VCPLOG_DEBUG("Refreshing physical monitor handles");

    auto enumerationResult = enumerateMonitors();

    if (!enumerationResult) {
        VCPLOG_DEBUG("Failed to refresh physical monitor handles: "
                     "msg={} code={}",
                     enumerationResult.error().message,
                     vcpilot::toString(enumerationResult.error().code));

        return std::unexpected(enumerationResult.error());
    }

    auto context = std::move(*enumerationResult);

    clearMonitorHandles();

    m_monitorHandles = std::move(context.handles);

    VCPLOG_DEBUG("Physical monitor handles refreshed successfully");

    return {};
}

void WindowsDdcBackend::clearMonitorHandles() {
    destroyMonitorHandles(m_monitorHandles);
}

void WindowsDdcBackend::destroyMonitorHandles(std::vector<MonitorHandleEntry>& handles) {
    for (auto& entry : handles) {
        if (!entry.physicalMonitors.empty()) {
            DestroyPhysicalMonitors(static_cast<DWORD>(entry.physicalMonitors.size()),
                                    entry.physicalMonitors.data());
        }
    }

    handles.clear();
}

// -----------------------------------------------------------------------------
// Internal VCP read
//
// Performs exactly one VCP operation using the currently cached handles.
// No refresh and no retry are performed here.
// -----------------------------------------------------------------------------

Result<VcpValue> WindowsDdcBackend::getVcpInternal(const std::string& monitorId,
                                                   std::uint8_t code) {

    auto it = std::find_if(
        m_monitorHandles.begin(), m_monitorHandles.end(),
        [&monitorId](const MonitorHandleEntry& entry) { return entry.id == monitorId; });

    if (it == m_monitorHandles.end()) {
        VCPLOG_DEBUG("getVcp failed: monitor not found");

        return std::unexpected(Error{
            .code = ErrorCode::MonitorNotFound,
            .message = "Monitor not found",
            .nativeCode = std::nullopt,
        });
    }

    if (it->physicalMonitors.empty()) {
        VCPLOG_DEBUG("getVcp failed: no physical monitor handles");

        return std::unexpected(Error{
            .code = ErrorCode::VcpReadFailed,
            .message = "Monitor has no physical monitor handles",
            .nativeCode = std::nullopt,
        });
    }

    DWORD currentValue = 0;
    DWORD maximumValue = 0;

    if (!GetVCPFeatureAndVCPFeatureReply(it->physicalMonitors[0].hPhysicalMonitor, code, nullptr,
                                         &currentValue, &maximumValue)) {

        const DWORD error = GetLastError();

        VCPLOG_DEBUG("getVcp failed: code=0x{:02X}, nativeError={}", code, error);

        return std::unexpected(Error{
            .code = ErrorCode::VcpReadFailed,
            .message = "Failed to read VCP feature",
            .nativeCode = error,
        });
    }

    VCPLOG_TRACE("getVcp succeeded: code=0x{:02X}, "
                 "current={}, maximum={}",
                 code, currentValue, maximumValue);

    return VcpValue{
        .current = static_cast<std::uint16_t>(currentValue),

        .maximum = static_cast<std::uint16_t>(maximumValue),
    };
}

// -----------------------------------------------------------------------------
// Public VCP read
//
// First try the cached handle.
// If it fails, the handle may have become stale because of a display topology
// change. Re-enumerate monitors and retry exactly once.
// -----------------------------------------------------------------------------

Result<VcpValue> WindowsDdcBackend::getVcp(const std::string& monitorId, std::uint8_t code) {

    VCPLOG_TRACE("getVcp: monitorId='{}', code=0x{:02X}", monitorId, code);

    auto result = getVcpInternal(monitorId, code);

    if (result) {
        return result;
    }

    const Error originalError = result.error();

    VCPLOG_DEBUG("VCP read failed for monitor '{}'. "
                 "Refreshing monitor handles and retrying once.",
                 monitorId);

    auto refreshResult = refreshMonitorHandles();

    if (!refreshResult) {
        VCPLOG_DEBUG("VCP read retry aborted because "
                     "monitor handle refresh failed");

        // The operation requested by the caller was the
        // VCP read, so preserve its original error.
        return std::unexpected(originalError);
    }

    VCPLOG_DEBUG("Retrying VCP read for monitor '{}'", monitorId);

    return getVcpInternal(monitorId, code);
}

// -----------------------------------------------------------------------------
// Internal VCP write
//
// Performs exactly one VCP operation using the currently cached handles.
// No refresh and no retry are performed here.
// -----------------------------------------------------------------------------

Result<void> WindowsDdcBackend::setVcpInternal(const std::string& monitorId, std::uint8_t code,
                                               std::uint16_t value) {

    auto it = std::find_if(
        m_monitorHandles.begin(), m_monitorHandles.end(),
        [&monitorId](const MonitorHandleEntry& entry) { return entry.id == monitorId; });

    if (it == m_monitorHandles.end()) {
        VCPLOG_DEBUG("setVcp failed: monitor not found");

        return std::unexpected(Error{
            .code = ErrorCode::MonitorNotFound,
            .message = "Monitor not found",
            .nativeCode = std::nullopt,
        });
    }

    if (it->physicalMonitors.empty()) {
        VCPLOG_DEBUG("setVcp failed: no physical monitor handles");

        return std::unexpected(Error{
            .code = ErrorCode::VcpWriteFailed,
            .message = "Monitor has no physical monitor handles",
            .nativeCode = std::nullopt,
        });
    }

    if (!SetVCPFeature(it->physicalMonitors[0].hPhysicalMonitor, code, static_cast<DWORD>(value))) {

        const DWORD error = GetLastError();

        VCPLOG_DEBUG("setVcp failed: code=0x{:02X}, "
                     "value={}, nativeError={}",
                     code, value, error);

        return std::unexpected(Error{
            .code = ErrorCode::VcpWriteFailed,
            .message = "Failed to write VCP feature",
            .nativeCode = error,
        });
    }

    VCPLOG_TRACE("setVcp succeeded: "
                 "code=0x{:02X}, value={}",
                 code, value);

    return {};
}

// -----------------------------------------------------------------------------
// Public VCP write
//
// First try the cached handle.
// On failure, refresh physical monitor handles and retry exactly once.
// -----------------------------------------------------------------------------

Result<void> WindowsDdcBackend::setVcp(const std::string& monitorId, std::uint8_t code,
                                       std::uint16_t value) {

    VCPLOG_TRACE("setVcp: monitorId='{}', "
                 "code=0x{:02X}, value={}",
                 monitorId, code, value);

    auto result = setVcpInternal(monitorId, code, value);

    if (result) {
        return result;
    }

    const Error originalError = result.error();

    VCPLOG_DEBUG("VCP write failed for monitor '{}'. "
                 "Refreshing monitor handles and retrying once.",
                 monitorId);

    auto refreshResult = refreshMonitorHandles();

    if (!refreshResult) {
        VCPLOG_DEBUG("VCP write retry aborted because "
                     "monitor handle refresh failed");

        return std::unexpected(originalError);
    }

    VCPLOG_DEBUG("Retrying VCP write for monitor '{}'", monitorId);

    return setVcpInternal(monitorId, code, value);
}

// -----------------------------------------------------------------------------
// Internal capabilities read
//
// Reads the raw MCCS capabilities string once using the currently cached
// physical monitor handle and parses it into MonitorCapabilities.
// No refresh and no retry are performed here.
// -----------------------------------------------------------------------------

Result<MonitorCapabilities>
WindowsDdcBackend::getCapabilitiesInternal(const std::string& monitorId) {

    auto it = std::find_if(
        m_monitorHandles.begin(), m_monitorHandles.end(),
        [&monitorId](const MonitorHandleEntry& entry) { return entry.id == monitorId; });

    if (it == m_monitorHandles.end()) {
        VCPLOG_DEBUG("getCapabilities failed: monitor not found");

        return std::unexpected(Error{
            .code = ErrorCode::MonitorNotFound,
            .message = "Monitor not found",
            .nativeCode = std::nullopt,
        });
    }

    if (it->physicalMonitors.empty()) {
        VCPLOG_DEBUG("getCapabilities failed: no physical monitor handles");

        return std::unexpected(Error{
            .code = ErrorCode::VcpReadFailed,
            .message = "Monitor has no physical monitor handles",
            .nativeCode = std::nullopt,
        });
    }

    const HANDLE physicalMonitor = it->physicalMonitors[0].hPhysicalMonitor;

    DWORD capabilitiesLength = 0;

    if (!GetCapabilitiesStringLength(physicalMonitor, &capabilitiesLength)) {

        const DWORD error = GetLastError();

        VCPLOG_DEBUG("GetCapabilitiesStringLength failed: nativeError={}", error);

        return std::unexpected(Error{
            .code = ErrorCode::VcpReadFailed,
            .message = "Failed to get monitor capabilities string length",
            .nativeCode = error,
        });
    }

    if (capabilitiesLength == 0) {
        VCPLOG_DEBUG("getCapabilities failed: capabilities string length is zero");

        return std::unexpected(Error{
            .code = ErrorCode::VcpReadFailed,
            .message = "Monitor returned an empty capabilities string",
            .nativeCode = std::nullopt,
        });
    }

    std::string capabilities(capabilitiesLength, '\0');

    if (!CapabilitiesRequestAndCapabilitiesReply(physicalMonitor, capabilities.data(),
                                                 capabilitiesLength)) {

        const DWORD error = GetLastError();

        VCPLOG_DEBUG("CapabilitiesRequestAndCapabilitiesReply failed: nativeError={}", error);

        return std::unexpected(Error{
            .code = ErrorCode::VcpReadFailed,
            .message = "Failed to read monitor capabilities",
            .nativeCode = error,
        });
    }

    if (!capabilities.empty() && capabilities.back() == '\0') {
        capabilities.pop_back();
    }

    VCPLOG_TRACE("Capabilities string received for monitor '{}': {}", monitorId, capabilities);

    return parseCapabilitiesString(capabilities);
}

// -----------------------------------------------------------------------------
// Public capabilities read
//
// First try the cached physical monitor handle.
// If it fails, refresh monitor handles and retry exactly once.
// -----------------------------------------------------------------------------

Result<MonitorCapabilities> WindowsDdcBackend::getCapabilities(const std::string& monitorId) {

    VCPLOG_TRACE("getCapabilities: monitorId='{}'", monitorId);

    auto result = getCapabilitiesInternal(monitorId);

    if (result) {
        return result;
    }

    const Error originalError = result.error();

    VCPLOG_DEBUG("Capabilities read failed for monitor '{}'. "
                 "Refreshing monitor handles and retrying once.",
                 monitorId);

    auto refreshResult = refreshMonitorHandles();

    if (!refreshResult) {
        VCPLOG_DEBUG("Capabilities retry aborted because "
                     "monitor handle refresh failed");

        return std::unexpected(originalError);
    }

    VCPLOG_DEBUG("Retrying capabilities read for monitor '{}'", monitorId);

    return getCapabilitiesInternal(monitorId);
}

} // namespace vcpilot