find_package(spdlog REQUIRED)

add_library(vcpilot_core
    src/logger.cpp
    src/platform/windows/windows_ddc_backend.cpp
    src/monitor_controller.cpp
)

target_compile_features(vcpilot_core
    PUBLIC
        cxx_std_23
)

target_include_directories(vcpilot_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vcpilot_core
    PRIVATE
        spdlog::spdlog
        Dxva2
        Setupapi
    PUBLIC
        fmt::fmt
)