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

Currently implemented:

- Monitor enumeration on Windows
- Monitor information retrieval
- EDID parsing for manufacturer, model, and serial information
- Multi-monitor position and primary monitor detection
- Physical monitor handle management
- DDC/CI VCP feature reading and writing
- Automatic handle refresh and retry on DDC/CI communication failure
- Monitor capabilities retrieval through MCCS
- Generic MCCS capabilities string parsing
- Runtime monitor capability model
- Monitor capability caching and cache lifecycle handling
- MCCS 2.0 VCP feature metadata catalog
- VCP feature access and type metadata
- Metadata for common discrete VCP values
- Generic VCP read/write API
- High-level monitor control API
- Brightness control
- Input source detection and switching
- Preservation of unknown and vendor-specific VCP features
- Catch2-based unit test infrastructure
- Unit tests for MCCS capability parsing and metadata
- Unit tests for monitor capability caching and cache lifecycle
- Unit tests for generic VCP controller delegation

Currently in development:

- Core backend stabilization and final review

Planned:

- Extended MCCS feature and value metadata
- Command-line interface
- Qt/QML graphical interface
- Capability-driven monitor controls
- Monitor profiles and custom modes

## Architecture

VCPilot separates high-level monitor control from platform-specific DDC/CI communication.

```text
Future User Interfaces
    |
    |-- CLI
    |-- Qt/QML GUI
    |
    v
MonitorController
    |
    v
IDdcBackend
    |
    v
WindowsDdcBackend
```

`MonitorController` exposes both high-level semantic controls and generic VCP access, allowing future interfaces to build controls dynamically from monitor capabilities and MCCS metadata.

Platform-specific DDC/CI communication is isolated behind `IDdcBackend`. Monitor models, MCCS capability parsing, metadata, and higher-level control logic remain independent from Windows-specific monitor APIs.

## Testing

VCPilot uses Catch2 for unit testing and CTest for test discovery and execution.

The current test suite covers:

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
- spdlog
- Catch2 3.15.3
- CTest

## Building

### Requirements

Make sure the following tools are installed and available from the command line:

- Visual Studio C++ Build Tools / MSVC
- CMake
- Ninja
- Conan 2
- Python

### Configure

From the repository root:

```bat
conan install . --output-folder=build --build=missing

cmake --preset conan-release
```

### Build

```bat
cmake --build --preset conan-release
```

## Platform

Windows is currently the primary development platform.

Windows monitor communication is implemented through the Windows DDC/CI APIs and isolated behind the backend interface. The architecture allows additional platform backends to be introduced in the future without coupling the core monitor-control model to Windows-specific APIs.

## License

VCPilot is licensed under the [MIT License](LICENSE).
