find_package(CLI11 REQUIRED)

add_executable(vcpilot_cli
    src/main.cpp
    src/list_command.cpp
    src/command_registry.cpp
    src/brightness_command.cpp
    src/contrast_command.cpp
    src/blue_black_level_command.cpp
    src/blue_gain_command.cpp
    src/color_preset_command.cpp
    src/contrast_command.cpp
    src/gamma_command.cpp
    src/green_black_level_command.cpp
    src/green_gain_command.cpp
    src/input_command.cpp
    src/mute_command.cpp
    src/power_command.cpp
    src/red_black_level_command.cpp
    src/red_gain_command.cpp
    src/saturation_command.cpp
    src/sharpness_command.cpp
    src/volume_command.cpp
    src/vcp_command.cpp
    src/info_command.cpp
)

target_include_directories(vcpilot_cli
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vcpilot_cli
    PRIVATE
        vcpilot_core
        CLI11::CLI11
)