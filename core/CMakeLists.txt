find_package(spdlog REQUIRED)

add_library(vcpilot_core
    src/Logger.cpp
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
    PUBLIC
        fmt::fmt
)