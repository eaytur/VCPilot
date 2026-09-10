cmake_minimum_required(VERSION 4.3.1)

project(vcpilot
        VERSION 0.1.0
        LANGUAGES CXX)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

add_subdirectory(core)
