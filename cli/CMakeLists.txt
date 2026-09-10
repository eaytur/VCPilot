find_package(CLI11 REQUIRED)

add_executable(vcpilot_cli
    src/main.cpp
    src/list_command.cpp
    src/command_registry.cpp
    src/brightness_command.cpp
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