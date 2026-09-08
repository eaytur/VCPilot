# VCPilot

VCPilot is a C++ application for controlling monitor settings through DDC/CI and VESA Monitor Control Command Set (MCCS).

The project is currently under development, starting with a Windows backend for monitor discovery and low-level monitor communication.

## Current Status

Currently implemented:

- Monitor enumeration on Windows
- Monitor information retrieval
- EDID parsing for manufacturer, model, and serial information
- Multi-monitor position and primary monitor detection
- DDC/CI VCP feature reading and writing
- High-level monitor control API
- Brightness control
- Input source detection and switching

DDC/CI VCP control is the next development step.

## Toolchain

- C++23
- MSVC
- CMake
- Ninja
- Conan 2

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

The core is designed so platform-specific monitor communication can remain isolated from the higher-level application logic.