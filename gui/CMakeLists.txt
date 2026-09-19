find_package(Qt6 REQUIRED COMPONENTS
    Quick
    QuickControls2
)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(vcpilot_gui
    src/main.cpp
    src/adapter/vcpilot_adapter.cpp
    src/adapter/vcpilot_adapter.hpp
    src/adapter/monitor_state_manager.cpp
    src/adapter/monitor_state_manager.hpp
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
        qml/components/MonitorStrip.qml
        qml/components/StatusIndicator.qml
        qml/components/AppFooter.qml
        qml/components/MonitorLayout.qml
        qml/components/InputSourceControl.qml
        qml/components/ScanProgressBar.qml
        qml/components/PictureControl.qml
        qml/components/CalibrationControl.qml
        qml/components/AppComboBox.qml
        qml/components/AudioControl.qml
        qml/components/VcpFeatureTable.qml
        qml/components/VcpTerminal.qml
        qml/components/AppTextField.qml
        qml/components/TitleBarButton.qml
        qml/pages/DashboardPage.qml
        qml/pages/ControlsPage.qml
        qml/pages/AdvancedPage.qml

    RESOURCES
        assets/icons/layout-dashboard.svg
        assets/icons/monitor.svg
        assets/icons/sun.svg
        assets/icons/moon.svg
        assets/icons/laptop.svg
        assets/icons/plus.svg
        assets/icons/forward.svg
        assets/icons/panels-top-left.svg
        assets/icons/controls.svg
        assets/icons/circle-alert.svg
        assets/icons/input-source.svg
        assets/icons/picture.svg
        assets/icons/calibration.svg
        assets/icons/image.svg
        assets/icons/volume-off.svg
        assets/icons/volume-2.svg
        assets/icons/send.svg
        assets/icons/terminal.svg
        assets/icons/wrench.svg
        assets/icons/window-minimize.svg
        assets/icons/window-maximize.svg
        assets/icons/window-restore.svg
        assets/icons/window-close.svg
        assets/images/contrast-calib.png
)

target_link_libraries(vcpilot_gui
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Concurrent
        vcpilot_core
)

target_compile_features(vcpilot_gui
    PRIVATE
        cxx_std_23
)