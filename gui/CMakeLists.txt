find_package(Qt6 REQUIRED COMPONENTS
    Quick
    QuickControls2
)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(vcpilot_gui
    src/main.cpp
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
    
    RESOURCES
        assets/icons/layout-dashboard.svg
        assets/icons/monitor.svg
        assets/icons/palette.svg
        assets/icons/power.svg
        assets/icons/refresh-cw.svg
        assets/icons/settings.svg
        assets/icons/sliders-horizontal.svg
        assets/icons/sun.svg
        assets/icons/volume-2.svg
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