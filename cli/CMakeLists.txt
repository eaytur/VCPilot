add_executable(vcpilot_cli 
    src/main.cpp
)


target_link_libraries(vcpilot_cli
    PRIVATE
        vcpilot_core
)