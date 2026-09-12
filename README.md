<p align="center">
  <img src="assets/vcpilot-logo.png" alt="VCPilot Logo" width="420">
</p>

<h1 align="center">VCPilot</h1>

<p align="center">
  A modern C++ monitor control application built on DDC/CI and VESA MCCS.
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

Current Status

Core Backend

Monitor enumeration on Windows

Fast monitor information retrieval

EDID parsing for manufacturer, model, and serial information

Multi-monitor position and primary monitor detection

Physical monitor handle management

Automatic handle refresh and retry on DDC/CI communication failure

DDC/CI VCP feature reading and writing

Monitor capabilities retrieval through MCCS

Generic MCCS capabilities string parsing

Runtime monitor capability model

Monitor capability caching and cache lifecycle handling

MCCS 2.0 VCP feature metadata catalog

VCP feature access and type metadata

Metadata for common discrete VCP values

Preservation of unknown and vendor-specific VCP features

Generic raw VCP read/write API

High-Level Monitor Control API

VCPilot Core provides semantic APIs for common monitor controls while retaining raw VCP access for advanced and vendor-specific operations.

Currently supported:

Brightness

Contrast

Input source

Audio volume

Audio mute

Power mode

Color preset

Sharpness

Saturation

Gamma

Red, green, and blue gain

Red, green, and blue black level

Command-Line Interface

The single-shot CLI is implemented using CLI11 and provides commands for the high-level monitor controls exposed by VCPilot Core.

It also includes:

Fast monitor listing

Monitor capability inspection

MCCS/VCP feature metadata display

Loading spinner during capability retrieval

Raw VCP read/write access

Decimal and hexadecimal VCP values

Command-based CLI architecture

Examples:

vcpilot_cli list
vcpilot_cli info -m 1

vcpilot_cli brightness -m 1
vcpilot_cli brightness -m 1 75

vcpilot_cli input -m 1 hdmi1
vcpilot_cli power -m 1 on

vcpilot_cli vcp -m 1 0x10
vcpilot_cli vcp -m 1 0x60 0x11

The raw vcp command acts as an escape hatch for advanced or vendor-specific monitor controls that do not have a high-level semantic API.

Qt / QML GUI

The graphical interface is currently under active development.

Implemented so far:

Qt 6 / QML application shell

Reusable application header

Dashboard navigation

Dark and light themes

Shared semantic theme tokens

Reusable UI components

Monitor card component

External monitor and built-in display visual distinction

Reusable segmented controls

Scrollable dashboard layout

Initial dashboard page structure

Native Windows window frame integration

The GUI currently uses static data while the backend-to-QML integration is being developed.

The next GUI milestone is to replace mock monitor data with real information from MonitorController.

Testing

Catch2-based unit test infrastructure

MCCS capability parser tests

MCCS feature and value metadata tests

Monitor capability cache lifecycle tests

Generic VCP controller delegation tests

High-level semantic monitor control tests

Architecture

VCPilot separates user interfaces, high-level monitor-control semantics, and platform-specific DDC/CI communication.

                 User Interfaces
               /                 \
              /                   \
            CLI               Qt/QML GUI
              \                   /
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

MonitorController is the central high-level interface of VCPilot Core.

Frontends express user intent through semantic operations such as brightness, input source, mute, or power control. The core translates those operations into the appropriate MCCS/VCP commands.

For advanced and vendor-specific functionality, the same controller also exposes generic getVcp() and setVcp() operations.

Platform-specific DDC/CI communication is isolated behind IDdcBackend.

Monitor models, MCCS capability parsing, metadata, caching, and semantic monitor-control logic remain independent from the Windows backend.

GUI Architecture

The Qt/QML interface is intentionally kept separate from the monitor-control backend.

QML UI
  |
  | user interaction
  v
GUI / Core bridge
  |
  v
MonitorController
  |
  v
VCPilot Core

The GUI is being developed incrementally.

Controls are added as backend functionality is integrated rather than building a large mock interface first.

The current integration plan is:

Monitor discovery
      |
      v
Monitor cards
      |
      v
Monitor selection
      |
      v
Input source
      |
      v
Brightness / Contrast / Audio
      |
      v
Capability-driven controls

This allows the interface to reflect what each monitor actually supports.

CLI

VCPilot uses a command-oriented CLI architecture built on CLI11.

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

Normal single-shot commands use the fast monitor-information path and directly attempt the requested operation without first querying the monitor's full MCCS capabilities.

Capability retrieval is performed explicitly by commands such as:

vcpilot_cli info -m 1

This avoids unnecessary DDC/CI capability queries during normal CLI operations.

Testing

VCPilot uses Catch2 for unit testing and CTest for test discovery and execution.

The test suite covers:

MCCS capabilities string parsing

Discrete VCP value parsing

Invalid and malformed capability data

Unknown VCP feature preservation

MCCS feature metadata lookup

MCCS discrete value metadata lookup

Monitor capability caching

Negative capability caching

Cache lifecycle across monitor disconnect and reconnect

Generic VCP read/write delegation through MonitorController

High-level semantic VCP mappings and operations

Run the test suite with:

ctest --preset conan-release --output-on-failure

Toolchain

C++23

Qt 6

QML

MSVC

CMake

Ninja

Conan 2

CLI11 2.5.0

spdlog

fmt

Catch2 3.15.3

CTest

Building

Requirements

Make sure the following tools are installed and available from the command line:

Visual Studio C++ Build Tools / MSVC

Qt 6

CMake

Ninja

Conan 2

Python

Install Dependencies

From the repository root:

conan install . --build=missing -s build_type=Release

Configure

cmake --preset conan-release

Build

cmake --build --preset conan-release

Test

ctest --preset conan-release --output-on-failure

CLI

After building, display the available CLI commands with:

vcpilot_cli --help

GUI

After building, run:

vcpilot_gui

The GUI currently provides the application shell, theme system, navigation, and dashboard foundation.

Backend-driven monitor data and controls are the next development step.

Platform

Windows is currently the primary development platform.

Monitor communication is implemented through the Windows DDC/CI APIs and isolated behind WindowsDdcBackend.

The backend abstraction allows additional platform implementations to be introduced in the future without coupling the core monitor-control model or user interfaces to Windows-specific APIs.

Planned

GUI Integration

Connect MonitorController to the Qt/QML frontend

Populate monitor cards from real monitor information

Real monitor selection

Input source switching

Brightness control

Contrast control

Audio controls

Capability-driven control visibility

Application Features

Monitor profiles and custom modes

System tray integration

Global hotkeys

Quick-access floating controls

Multi-monitor profile application

Project Direction

VCPilot is being developed as a general-purpose monitor control application rather than a monitor-specific utility.

The long-term goal is to provide a modern Qt/QML desktop interface where users can create custom monitor profiles such as gaming, coding, reading, or console modes.

Profiles will be able to combine multiple monitor settings and input-source changes into a single action.

The same core remains available to the CLI for scripting, diagnostics, and advanced raw VCP access.

The graphical interface is designed to be capability-driven so that controls can adapt to the features actually exposed by each connected display.

License

VCPilot is licensed under the MIT License.