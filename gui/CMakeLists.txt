find_package(Qt6 REQUIRED COMPONENTS
    Quick
    QuickControls2
)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(vcpilot_gui
    src/main.cpp
    src/adapter/vcpilot_adapter.cpp
    src/adapter/vcpilot_adapter.hpp
)

target_include_directories(vcpilot_gui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_source_files_properties(
    qml/theme/Theme.qml
    PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE
)


qt_add_qml_module(vcpilot_gui
    URI VCPilot
    VERSION 1.0

    QML_FILES
        qml/Main.qml
        qml/theme/Theme.qml
        qml/components/SectionCard.qml
        qml/components/ControlSlider.qml
        qml/components/SegmentedControl.qml
        qml/components/MonitorCard.qml
        qml/components/AppIcon.qml
        qml/components/IconButton.qml
        qml/components/AppButton.qml
        qml/components/NavigationButton.qml
        qml/components/NavigationBar.qml
        qml/components/AppHeader.qml
        qml/components/AppSwitch.qml
        qml/components/ActionCard.qml
        qml/pages/DashboardPage.qml

    RESOURCES
        assets/icons/layout-dashboard.svg
        assets/icons/monitor.svg
        assets/icons/sun.svg
        assets/icons/moon.svg
        assets/icons/laptop.svg
        assets/icons/plus.svg
)

target_link_libraries(vcpilot_gui
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        vcpilot_core
)

target_compile_features(vcpilot_gui
    PRIVATE
        cxx_std_23
)