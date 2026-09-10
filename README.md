<p align="center">
  <img src="assets/vcpilot-logo.png" alt="VCPilot Logo" width="420">
</p>

<h1 align="center">VCPilot</h1>

<p align="center">
  A modern C++ monitor control application built on DDC/CI and VESA MCCS.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus&logoColor=white">
  <img src="https://img.shields.io/badge/Windows-0078D4?logo=windows11&logoColor=white">
  <img src="https://img.shields.io/badge/CMake-064F8C?logo=cmake&logoColor=white">
  <img src="https://img.shields.io/badge/Conan-6699CB?logo=conan&logoColor=white">
  <img src="https://img.shields.io/badge/MSVC-5C2D91?logo=visualstudio&logoColor=white">
  <img src="https://img.shields.io/badge/Catch2-2C3E50">
</p>

## Current Status

### Core Backend

- Monitor enumeration on Windows
- Fast monitor information retrieval
- EDID parsing for manufacturer, model, and serial information
- Multi-monitor position and primary monitor detection
- Physical monitor handle management
- Automatic handle refresh and retry on DDC/CI communication failure
- DDC/CI VCP feature reading and writing
- Monitor capabilities retrieval through MCCS
- Generic MCCS capabilities string parsing
- Runtime monitor capability model
- Monitor capability caching and cache lifecycle handling
- MCCS 2.0 VCP feature metadata catalog
- VCP feature access and type metadata
- Metadata for common discrete VCP values
- Preservation of unknown and vendor-specific VCP features
- Generic raw VCP read/write API

### High-Level Monitor Control API

VCPilot Core provides semantic APIs for common monitor controls while retaining raw VCP access for advanced and vendor-specific operations.

Currently supported:

- Brightness
- Contrast
- Input source
- Audio volume
- Audio mute
- Power mode
- Color preset
- Sharpness
- Saturation
- Gamma
- Red, green, and blue gain
- Red, green, and blue black level

### Command-Line Interface

The single-shot CLI is implemented using CLI11 and provides commands for the high-level monitor controls exposed by VCPilot Core.

It also includes:

- Fast monitor listing
- Monitor capability inspection
- MCCS/VCP feature metadata display
- Loading spinner during capability retrieval
- Raw VCP read/write access
- Decimal and hexadecimal VCP values
- Command-based CLI architecture

Examples:

```bat
vcpilot_cli list
vcpilot_cli info -m 1

vcpilot_cli brightness -m 1
vcpilot_cli brightness -m 1 75

vcpilot_cli input -m 1 hdmi1
vcpilot_cli power -m 1 on

vcpilot_cli vcp -m 1 0x10
vcpilot_cli vcp -m 1 0x60 0x11
```

The raw `vcp` command acts as an escape hatch for advanced or vendor-specific monitor controls that do not have a high-level semantic API.

### Testing

- Catch2-based unit test infrastructure
- MCCS capability parser tests
- MCCS feature and value metadata tests
- Monitor capability cache lifecycle tests
- Generic VCP controller delegation tests
- High-level semantic monitor control tests

### Planned

- Qt/QML graphical interface
- Capability-driven monitor controls
- Monitor profiles and custom modes
- System tray integration
- Global hotkeys
- Quick-access floating mode controls

## Architecture

VCPilot separates user interfaces, high-level monitor-control semantics, and platform-specific DDC/CI communication.

```text
                 User Interfaces
              /                  \
            CLI              Qt/QML GUI
              \                  /
               \                /
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

`MonitorController` is the central high-level interface of VCPilot Core.

Frontends express user intent through semantic operations such as brightness, input source, mute, or power control. The core translates those operations into the appropriate MCCS/VCP commands.

For advanced and vendor-specific functionality, the same controller also exposes generic `getVcp()` and `setVcp()` operations.

Platform-specific DDC/CI communication is isolated behind `IDdcBackend`. Monitor models, MCCS capability parsing, metadata, caching, and semantic monitor-control logic remain independent from the Windows backend.

## CLI

VCPilot uses a command-oriented CLI architecture built on CLI11.

```text
CLI11
  |
CommandRegistry
  |
ICommand
  |
  +-- ListCommand
  +-- InfoCommand
  +-- BrightnessCommand
  +-- ContrastCommand
  +-- InputCommand
  +-- PowerCommand
  +-- ...
  +-- VcpCommand
  |
MonitorController
```

Normal single-shot commands use the fast monitor-information path and directly attempt the requested operation without first querying the monitor's full MCCS capabilities.

Capability retrieval is performed explicitly by commands such as:

```bat
vcpilot_cli info -m 1
```

This avoids unnecessary DDC/CI capability queries during normal CLI operations.

## Testing

VCPilot uses Catch2 for unit testing and CTest for test discovery and execution.

The test suite covers:

- MCCS capabilities string parsing
- Discrete VCP value parsing
- Invalid and malformed capability data
- Unknown VCP feature preservation
- MCCS feature metadata lookup
- MCCS discrete value metadata lookup
- Monitor capability caching
- Negative capability caching
- Cache lifecycle across monitor disconnect and reconnect
- Generic VCP read/write delegation through `MonitorController`
- High-level semantic VCP mappings and operations

Run the test suite with:

```bat
ctest --preset conan-release --output-on-failure
```

## Toolchain

- C++23
- MSVC
- CMake
- Ninja
- Conan 2
- CLI11 2.5.0
- spdlog
- fmt
- Catch2 3.15.3
- CTest

Qt 6 / QML is planned for the graphical interface.

## Building

### Requirements

Make sure the following tools are installed and available from the command line:

- Visual Studio C++ Build Tools / MSVC
- CMake
- Ninja
- Conan 2
- Python

### Install Dependencies

From the repository root:

```bat
conan install . --build=missing -s build_type=Release
```

### Configure

```bat
cmake --preset conan-release
```

### Build

```bat
cmake --build --preset conan-release
```

### Test

```bat
ctest --preset conan-release --output-on-failure
```

### CLI

After building, the CLI executable can be found in the configured build output directory.

Display available commands with:

```bat
vcpilot_cli --help
```

## Platform

Windows is currently the primary development platform.

Monitor communication is implemented through the Windows DDC/CI APIs and isolated behind `WindowsDdcBackend`.

The backend abstraction allows additional platform implementations to be introduced in the future without coupling the core monitor-control model or user interfaces to Windows-specific APIs.

## Project Direction

VCPilot is being developed as a general-purpose monitor control application rather than a monitor-specific utility.

The long-term goal is to provide a modern Qt/QML desktop interface where users can create custom monitor profiles such as gaming, coding, reading, or console modes. Profiles will be able to combine multiple monitor settings and input-source changes into a single action.

The same core remains available to the CLI for scripting, diagnostics, and advanced raw VCP access.

## License

VCPilot is licensed under the [MIT License](LICENSE).