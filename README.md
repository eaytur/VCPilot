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
- High-level monitor control API
- Brightness control
- Input source detection and switching

Currently in development:

- MCCS VCP feature metadata/catalog
- Capability-driven monitor controls

Planned:

- Extended MCCS feature support
- Command-line interface
- Qt/QML graphical interface
- Monitor profiles and custom modes

## Architecture

VCPilot separates high-level monitor control from platform-specific DDC/CI communication.

```text
CLI / GUI
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

The core application logic remains independent from Windows-specific monitor APIs, allowing additional platform backends to be introduced in the future.

## Toolchain

- C++23
- MSVC
- CMake
- Ninja
- Conan 2
- spdlog

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

### Run

```bat
.\build\cli\vcpilot_cli.exe
```

## Platform

Windows is currently the primary development platform.

Platform-specific monitor communication is isolated behind the backend interface, while monitor models, MCCS capability handling, and higher-level control logic remain in the core.