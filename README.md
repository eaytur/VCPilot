<p align="center">
  <img src="assets/vcpilot-banner.png" alt="VCPilot" width="100%">
</p>

<p align="center">
  <strong>VCPilot</strong> is a modern Windows monitor control application built with C++23 and Qt 6 / QML.<br>
  Control brightness, contrast, input source, audio, and more through DDC/CI (MCCS).
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus&logoColor=white">
  <img src="https://img.shields.io/badge/Qt-6-41CD52?logo=qt&logoColor=white">
  <img src="https://img.shields.io/badge/QML-UI-41CD52?logo=qt&logoColor=white">
  <img src="https://img.shields.io/badge/Windows-0078D4?logo=windows11&logoColor=white">
  <img src="https://img.shields.io/badge/CMake-064F8C?logo=cmake&logoColor=white">
  <img src="https://img.shields.io/badge/Conan-6699CB?logo=conan&logoColor=white">
  <img src="https://img.shields.io/badge/MSVC-5C2D91?logo=visualstudio&logoColor=white">
  <img src="https://img.shields.io/badge/Catch2-2C3E50">
</p>

<p align="center">
  <img src="assets/vcpilot-showcase.png" alt="VCPilot application showcase" width="100%">
</p>

---

## Overview

VCPilot separates three concerns: platform-specific DDC/CI communication, a semantic monitor-control core, and two user interfaces (CLI and Qt/QML GUI) built on top of it.

```
                 User Interfaces
               /                 \
             CLI               Qt/QML GUI
               \                 /
                MonitorController
                       |
             Semantic Monitor API
                       |
              Generic VCP Access
                       |
                  IDdcBackend
                       |
               WindowsDdcBackend
                       |
              Windows DDC/CI APIs
```

`MonitorController` is the central high-level interface: frontends express intent (brightness, input source, mute, power, ...) and the core translates it into MCCS/VCP commands. Raw `getVcp()` / `setVcp()` stay available for advanced or vendor-specific access. Windows-specific code is isolated behind `IDdcBackend`, so the core, capability model, and metadata stay platform-independent.

## Core Backend

- Monitor enumeration on Windows, with internal/external distinction and primary monitor detection
- Monitor position, bounds, and resolution
- EDID-based manufacturer / model / serial info
- Physical monitor handle management, with automatic refresh + retry on DDC/CI failure
- Generic `getVcp()` / `setVcp()`
- MCCS capabilities string retrieval and parsing into a `MonitorCapabilities` model
- Capability caching (including negative caching) with connect/disconnect lifecycle handling
- MCCS 2.0 VCP metadata catalog: feature access/type metadata, known discrete values, and preservation of unknown/vendor-specific features

## Semantic Monitor API

Exposed through `MonitorController`: brightness, contrast, input source, volume, mute, power mode, color preset, sharpness, saturation, gamma, RGB gain, RGB black level. Raw VCP access remains available as an escape hatch for anything not covered semantically.

## Runtime State

`MonitorState` tracks current input, brightness/max, contrast/max, volume/max, and mute. `MonitorStateManager` polls this asynchronously and keeps the GUI in sync, with all DDC/CI work kept off the UI thread.

## CLI

Built with CLI11, command-based architecture:

```
vcpilot_cli list
vcpilot_cli info -m 1
vcpilot_cli brightness -m 1 75
vcpilot_cli input -m 1 hdmi1
vcpilot_cli power -m 1 on
vcpilot_cli vcp -m 1 0x10
vcpilot_cli vcp -m 1 0x60 0x11
```

Also included: capability/metadata display, a loading spinner during capability retrieval, and raw VCP read/write with decimal or hex values. Normal commands use a fast info path and only query full MCCS capabilities when explicitly asked (e.g. `info`).

## Qt / QML GUI

**Dashboard**
- Async monitor discovery, monitor cards from real backend data
- Windows-coordinate-based layout visualization
- Primary / internal / controllable monitor state
- Dark/light theme via a shared semantic theme system
- Custom frameless header (drag, minimize, maximize/restore, close, double-click maximize)

**Controls**
- Brightness, contrast, input source, volume, mute for the selected monitor
- Capability-aware handling, backend state sync, unsupported/internal-display handling
- Calibration panel with a contrast test pattern

**Advanced**
- MCCS version, full VCP feature table (code, name, access, type, supported values), vendor-specific/unknown feature support
- Raw VCP terminal: `getvcp`, `setvcp`, `help`, `clear`, hex/decimal parsing, command history (↑/↓ navigation, duplicate suppression, 50-command limit), auto-scroll

```text
Controls
├── Calibration
├── Picture (Brightness, Contrast)
├── Input Source
└── Audio (Volume, Mute)
```

More specialized controls (RGB gain, black level, gamma, saturation, sharpness, color presets, power) stay available via Core and the CLI without being forced into this page.

### GUI Architecture

```text
QML UI
   |
   v
VCPilotAdapter
   |
   +-- monitor discovery / commands
   |
   +-- MonitorStateManager -- periodic runtime state polling
   |
   v
MonitorController
   |
   v
VCPilot Core
```

## Testing

Catch2 + CTest, covering:

- MCCS capability parsing (incl. invalid/malformed data)
- Discrete value and feature metadata lookup
- Unknown VCP feature preservation
- Capability caching, negative caching, and disconnect/reconnect lifecycle
- Generic VCP delegation and semantic control mappings

```
ctest --preset conan-release --output-on-failure
```

## Toolchain

C++23 · Qt 6 · QML · MSVC · CMake · Ninja · Conan 2 · CLI11 2.5.0 · spdlog · fmt · Catch2 3.15.3

## Building

**Requirements:** MSVC / VS C++ Build Tools, Qt 6, CMake, Ninja, Conan 2, Python

```
conan install . --build=missing -s build_type=Release
cmake --preset conan-release
cmake --build --preset conan-release
ctest --preset conan-release --output-on-failure
```

Run the CLI: `vcpilot_cli --help`
Run the GUI: `vcpilot_gui`

## Platform

Windows is the current target. Monitor communication goes through the Windows DDC/CI APIs, isolated behind `WindowsDdcBackend`, so additional platform backends can be added later without touching the core or UI.

## Planned

- Capability-driven visibility for individual controls
- Additional calibration patterns, improved command feedback
- Monitor profiles (gaming/coding/reading/console modes) combining multiple settings and input changes
- System tray integration, global hotkeys, quick-access floating controls
- Multi-monitor profile application

## License

MIT License.