#include "vcpilot/logger.hpp"
#include "vcpilot/monitor_controller.hpp"

#include <chrono>
#include <thread>

int main() {
    vcpilot::log::initialize();

    vcpilot::MonitorController controller;

    auto monitorsResult = controller.listMonitors();

    if (!monitorsResult) {
        VCPLOG_ERROR("Failed to list monitors: message='{}', nativeError={}",
                     monitorsResult.error().message, monitorsResult.error().nativeCode.value_or(0));
        return 1;
    }

    for (const auto& monitor : *monitorsResult) {
        VCPLOG_INFO("Monitor: id='{}', manufacturer='{}', model='{}', serial='{}', "
                    "primary={}, {}x{}, x={}, y={}",
                    monitor.id, monitor.manufacturer, monitor.model, monitor.serial,
                    monitor.isPrimary, monitor.bounds.width, monitor.bounds.height,
                    monitor.bounds.x, monitor.bounds.y);

        // Brightness read
        auto brightnessResult = controller.getBrightness(monitor.id);

        if (!brightnessResult) {
            VCPLOG_ERROR("Failed to get brightness: msg={} code={}",
                         brightnessResult.error().message,
                         vcpilot::toString(brightnessResult.error().code));
        } else {
            VCPLOG_INFO("Brightness: current={} maximum={}", brightnessResult->current,
                        brightnessResult->maximum);

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            auto setBrightnessResult =
                controller.setBrightness(monitor.id, brightnessResult->current);

            if (!setBrightnessResult) {
                VCPLOG_ERROR("Failed to set brightness: msg={} code={}",
                             setBrightnessResult.error().message,
                             vcpilot::toString(setBrightnessResult.error().code));
            } else {
                VCPLOG_INFO("Brightness write succeeded");
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        auto inputResult = controller.getInputSource(monitor.id);

        if (!inputResult) {
            VCPLOG_ERROR("Failed to get input source: msg={} code={}", inputResult.error().message,
                         vcpilot::toString(inputResult.error().code));
            continue;
        }

        VCPLOG_INFO("Input source: {}", vcpilot::toString(*inputResult));

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        auto setInputResult = controller.setInputSource(monitor.id, *inputResult);

        if (!setInputResult) {
            VCPLOG_ERROR("Failed to set input source: msg={} code={}",
                         setInputResult.error().message,
                         vcpilot::toString(setInputResult.error().code));
        } else {
            VCPLOG_INFO("Input source write succeeded");
        }
    }

    return 0;
}