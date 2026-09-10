find_package(Catch2 3 REQUIRED)

add_executable(vcpilot_tests
    test_monitor_capabilities.cpp
    test_mccs_catalog.cpp
    test_monitor_controller.cpp
)

target_link_libraries(vcpilot_tests
    PRIVATE
        vcpilot_core
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(vcpilot_tests)