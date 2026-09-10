#include "vcpilot/iddc_backend.hpp"
#include "vcpilot/monitor_controller.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <vector>

namespace {

class FakeDdcBackend final : public vcpilot::IDdcBackend {
  public:
    vcpilot::Result<std::vector<vcpilot::MonitorInfo>> listMonitors() override {
        return monitors;
    }

    vcpilot::Result<vcpilot::MonitorCapabilities> getCapabilities(const std::string&) override {
        ++capabilitiesCallCount;

        if (failCapabilities) {
            return std::unexpected(
                vcpilot::Error{vcpilot::ErrorCode::VcpReadFailed, "Fake capabilities failure"});
        }

        return vcpilot::MonitorCapabilities{};
    }

    vcpilot::Result<vcpilot::VcpValue> getVcp(const std::string& monitorId,
                                              std::uint8_t code) override {

        ++getVcpCallCount;
        lastGetVcpMonitorId = monitorId;
        lastGetVcpCode = code;

        return getVcpResult;
    }

    vcpilot::Result<void> setVcp(const std::string& monitorId, std::uint8_t code,
                                 std::uint16_t value) override {

        ++setVcpCallCount;
        lastSetVcpMonitorId = monitorId;
        lastSetVcpCode = code;
        lastSetVcpValue = value;

        return {};
    }

    std::vector<vcpilot::MonitorInfo> monitors;

    int capabilitiesCallCount = 0;

    bool failCapabilities = false;

    int getVcpCallCount = 0;
    std::string lastGetVcpMonitorId;
    std::uint8_t lastGetVcpCode = 0;
    vcpilot::VcpValue getVcpResult{};

    int setVcpCallCount = 0;
    std::string lastSetVcpMonitorId;
    std::uint8_t lastSetVcpCode = 0;
    std::uint16_t lastSetVcpValue = 0;
};

} // namespace

TEST_CASE("MonitorController returns enumerated monitors") {
    auto backend = std::make_unique<FakeDdcBackend>();

    vcpilot::MonitorInfo monitor;
    monitor.id = "monitor-1";

    backend->monitors.push_back(monitor);

    vcpilot::MonitorController controller(std::move(backend));

    const auto monitors = controller.getMonitors();

    REQUIRE(monitors.has_value());
    REQUIRE(monitors->size() == 1);
    REQUIRE(monitors->at(0).info.id == "monitor-1");
}

TEST_CASE("MonitorController caches monitor capabilities") {
    auto backend = std::make_unique<FakeDdcBackend>();

    vcpilot::MonitorInfo monitor;
    monitor.id = "monitor-1";

    backend->monitors.push_back(monitor);

    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    const auto first = controller.getMonitors();
    REQUIRE(first.has_value());

    const auto second = controller.getMonitors();
    REQUIRE(second.has_value());

    REQUIRE(backendPtr->capabilitiesCallCount == 1);
}

TEST_CASE("MonitorController negative caches unavailable capabilities") {
    auto backend = std::make_unique<FakeDdcBackend>();

    vcpilot::MonitorInfo monitor;
    monitor.id = "monitor-1";

    backend->monitors.push_back(monitor);
    backend->failCapabilities = true;

    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    const auto first = controller.getMonitors();

    REQUIRE(first.has_value());
    REQUIRE(first->size() == 1);
    REQUIRE_FALSE(first->at(0).capabilities.has_value());

    const auto second = controller.getMonitors();

    REQUIRE(second.has_value());
    REQUIRE(second->size() == 1);
    REQUIRE_FALSE(second->at(0).capabilities.has_value());

    REQUIRE(backendPtr->capabilitiesCallCount == 1);
}

TEST_CASE("MonitorController refreshes capabilities after monitor reconnect") {
    auto backend = std::make_unique<FakeDdcBackend>();

    vcpilot::MonitorInfo monitor;
    monitor.id = "monitor-1";

    backend->monitors.push_back(monitor);

    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    // First discovery -> capabilities queried.
    const auto first = controller.getMonitors();

    REQUIRE(first.has_value());
    REQUIRE(first->size() == 1);
    REQUIRE(backendPtr->capabilitiesCallCount == 1);

    // Second discovery -> capabilities come from cache.
    const auto second = controller.getMonitors();

    REQUIRE(second.has_value());
    REQUIRE(backendPtr->capabilitiesCallCount == 1);

    // Monitor disconnected.
    backendPtr->monitors.clear();

    const auto disconnected = controller.getMonitors();

    REQUIRE(disconnected.has_value());
    REQUIRE(disconnected->empty());

    // Same monitor reconnects.
    backendPtr->monitors.push_back(monitor);

    const auto reconnected = controller.getMonitors();

    REQUIRE(reconnected.has_value());
    REQUIRE(reconnected->size() == 1);

    // Cache entry must have been removed on disconnect,
    // so capabilities must be queried again.
    REQUIRE(backendPtr->capabilitiesCallCount == 2);
}

TEST_CASE("MonitorController keeps monitor when capabilities are unavailable") {
    auto backend = std::make_unique<FakeDdcBackend>();

    vcpilot::MonitorInfo monitor;
    monitor.id = "monitor-1";

    backend->monitors.push_back(monitor);
    backend->failCapabilities = true;

    vcpilot::MonitorController controller(std::move(backend));

    const auto monitors = controller.getMonitors();

    REQUIRE(monitors.has_value());
    REQUIRE(monitors->size() == 1);

    REQUIRE(monitors->at(0).info.id == "monitor-1");
    REQUIRE_FALSE(monitors->at(0).capabilities.has_value());
}

TEST_CASE("MonitorController delegates generic VCP reads to backend") {
    auto backend = std::make_unique<FakeDdcBackend>();

    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    const auto result = controller.getVcp("monitor-1", 0x10);

    REQUIRE(result.has_value());

    REQUIRE(backendPtr->getVcpCallCount == 1);
    REQUIRE(backendPtr->lastGetVcpMonitorId == "monitor-1");
    REQUIRE(backendPtr->lastGetVcpCode == 0x10);
}

TEST_CASE("MonitorController delegates generic VCP writes to backend") {
    auto backend = std::make_unique<FakeDdcBackend>();

    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    const auto result = controller.setVcp("monitor-1", 0x10, 75);

    REQUIRE(result.has_value());

    REQUIRE(backendPtr->setVcpCallCount == 1);
    REQUIRE(backendPtr->lastSetVcpMonitorId == "monitor-1");
    REQUIRE(backendPtr->lastSetVcpCode == 0x10);
    REQUIRE(backendPtr->lastSetVcpValue == 75);
}

TEST_CASE("MonitorController scalar getters use correct VCP codes") {
    auto backend = std::make_unique<FakeDdcBackend>();
    auto* backendPtr = backend.get();

    backend->getVcpResult = vcpilot::VcpValue{
        .current = 50,
        .maximum = 100,
    };

    vcpilot::MonitorController controller(std::move(backend));

    SECTION("Brightness") {
        REQUIRE(controller.getBrightness("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x10);
    }

    SECTION("Contrast") {
        REQUIRE(controller.getContrast("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x12);
    }

    SECTION("Red gain") {
        REQUIRE(controller.getRedGain("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x16);
    }

    SECTION("Green gain") {
        REQUIRE(controller.getGreenGain("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x18);
    }

    SECTION("Blue gain") {
        REQUIRE(controller.getBlueGain("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x1A);
    }

    SECTION("Volume") {
        REQUIRE(controller.getVolume("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x62);
    }

    SECTION("Red black level") {
        REQUIRE(controller.getRedBlackLevel("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x6C);
    }

    SECTION("Green black level") {
        REQUIRE(controller.getGreenBlackLevel("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x6E);
    }

    SECTION("Blue black level") {
        REQUIRE(controller.getBlueBlackLevel("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x70);
    }

    SECTION("Gamma") {
        REQUIRE(controller.getGamma("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x72);
    }

    SECTION("Sharpness") {
        REQUIRE(controller.getSharpness("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x87);
    }

    SECTION("Saturation") {
        REQUIRE(controller.getSaturation("monitor-1"));
        REQUIRE(backendPtr->lastGetVcpCode == 0x8A);
    }
}

TEST_CASE("MonitorController scalar setters use correct VCP codes and values") {
    auto backend = std::make_unique<FakeDdcBackend>();
    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    SECTION("Brightness") {
        REQUIRE(controller.setBrightness("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x10);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }

    SECTION("Contrast") {
        REQUIRE(controller.setContrast("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x12);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }

    SECTION("Red gain") {
        REQUIRE(controller.setRedGain("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x16);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }

    SECTION("Green gain") {
        REQUIRE(controller.setGreenGain("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x18);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }

    SECTION("Blue gain") {
        REQUIRE(controller.setBlueGain("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x1A);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }

    SECTION("Volume") {
        REQUIRE(controller.setVolume("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x62);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }

    SECTION("Red black level") {
        REQUIRE(controller.setRedBlackLevel("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x6C);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }

    SECTION("Green black level") {
        REQUIRE(controller.setGreenBlackLevel("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x6E);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }

    SECTION("Blue black level") {
        REQUIRE(controller.setBlueBlackLevel("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x70);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }

    SECTION("Gamma") {
        REQUIRE(controller.setGamma("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x72);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }

    SECTION("Sharpness") {
        REQUIRE(controller.setSharpness("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x87);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }

    SECTION("Saturation") {
        REQUIRE(controller.setSaturation("monitor-1", 75));
        REQUIRE(backendPtr->lastSetVcpCode == 0x8A);
        REQUIRE(backendPtr->lastSetVcpValue == 75);
    }
}

TEST_CASE("MonitorController maps MCCS mute values") {
    auto backend = std::make_unique<FakeDdcBackend>();
    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    SECTION("Muted") {
        backendPtr->getVcpResult = {
            .current = 0x01,
            .maximum = 0,
        };

        const auto result = controller.getMute("monitor-1");

        REQUIRE(result);
        REQUIRE(*result == true);
        REQUIRE(backendPtr->lastGetVcpCode == 0x8D);
    }

    SECTION("Unmuted") {
        backendPtr->getVcpResult = {
            .current = 0x02,
            .maximum = 0,
        };

        const auto result = controller.getMute("monitor-1");

        REQUIRE(result);
        REQUIRE(*result == false);
        REQUIRE(backendPtr->lastGetVcpCode == 0x8D);
    }
}

TEST_CASE("MonitorController maps mute state to MCCS values") {
    auto backend = std::make_unique<FakeDdcBackend>();
    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    SECTION("Mute") {
        REQUIRE(controller.setMute("monitor-1", true));
        REQUIRE(backendPtr->lastSetVcpCode == 0x8D);
        REQUIRE(backendPtr->lastSetVcpValue == 0x01);
    }

    SECTION("Unmute") {
        REQUIRE(controller.setMute("monitor-1", false));
        REQUIRE(backendPtr->lastSetVcpCode == 0x8D);
        REQUIRE(backendPtr->lastSetVcpValue == 0x02);
    }
}

TEST_CASE("MonitorController maps MCCS power values") {
    auto backend = std::make_unique<FakeDdcBackend>();
    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    SECTION("On") {
        backendPtr->getVcpResult.current = 0x01;
        const auto result = controller.getPowerMode("monitor-1");

        REQUIRE(result);
        REQUIRE(*result == vcpilot::PowerMode::On);
    }

    SECTION("Standby") {
        backendPtr->getVcpResult.current = 0x02;
        const auto result = controller.getPowerMode("monitor-1");

        REQUIRE(result);
        REQUIRE(*result == vcpilot::PowerMode::Standby);
    }

    SECTION("Suspend") {
        backendPtr->getVcpResult.current = 0x03;
        const auto result = controller.getPowerMode("monitor-1");

        REQUIRE(result);
        REQUIRE(*result == vcpilot::PowerMode::Suspend);
    }

    SECTION("Off") {
        backendPtr->getVcpResult.current = 0x04;
        const auto result = controller.getPowerMode("monitor-1");

        REQUIRE(result);
        REQUIRE(*result == vcpilot::PowerMode::Off);
    }
}

TEST_CASE("MonitorController maps power modes to MCCS values") {
    auto backend = std::make_unique<FakeDdcBackend>();
    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    SECTION("On") {
        REQUIRE(controller.setPowerMode("monitor-1", vcpilot::PowerMode::On));
        REQUIRE(backendPtr->lastSetVcpCode == 0xD6);
        REQUIRE(backendPtr->lastSetVcpValue == 0x01);
    }

    SECTION("Standby") {
        REQUIRE(controller.setPowerMode("monitor-1", vcpilot::PowerMode::Standby));
        REQUIRE(backendPtr->lastSetVcpCode == 0xD6);
        REQUIRE(backendPtr->lastSetVcpValue == 0x02);
    }

    SECTION("Suspend") {
        REQUIRE(controller.setPowerMode("monitor-1", vcpilot::PowerMode::Suspend));
        REQUIRE(backendPtr->lastSetVcpCode == 0xD6);
        REQUIRE(backendPtr->lastSetVcpValue == 0x03);
    }

    SECTION("Off") {
        REQUIRE(controller.setPowerMode("monitor-1", vcpilot::PowerMode::Off));
        REQUIRE(backendPtr->lastSetVcpCode == 0xD6);
        REQUIRE(backendPtr->lastSetVcpValue == 0x04);
    }
}

TEST_CASE("MonitorController rejects unknown mute value") {
    auto backend = std::make_unique<FakeDdcBackend>();
    auto* backendPtr = backend.get();

    backendPtr->getVcpResult.current = 0x99;

    vcpilot::MonitorController controller(std::move(backend));

    const auto result = controller.getMute("monitor-1");

    REQUIRE_FALSE(result);
    REQUIRE(result.error().code == vcpilot::ErrorCode::Unknown);
}

TEST_CASE("MonitorController maps MCCS color preset values") {
    auto backend = std::make_unique<FakeDdcBackend>();
    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    SECTION("sRGB") {
        backendPtr->getVcpResult.current = 0x01;

        const auto result = controller.getColorPreset("monitor-1");

        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::SRgb);
        REQUIRE(backendPtr->lastGetVcpCode == 0x14);
    }

    SECTION("Display Native") {
        backendPtr->getVcpResult.current = 0x02;

        const auto result = controller.getColorPreset("monitor-1");

        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::DisplayNative);
        REQUIRE(backendPtr->lastGetVcpCode == 0x14);
    }

    SECTION("4000K") {
        backendPtr->getVcpResult.current = 0x03;
        const auto result = controller.getColorPreset("monitor-1");
        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::K4000);
    }

    SECTION("5000K") {
        backendPtr->getVcpResult.current = 0x04;
        const auto result = controller.getColorPreset("monitor-1");
        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::K5000);
    }

    SECTION("6500K") {
        backendPtr->getVcpResult.current = 0x05;
        const auto result = controller.getColorPreset("monitor-1");
        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::K6500);
    }

    SECTION("7500K") {
        backendPtr->getVcpResult.current = 0x06;
        const auto result = controller.getColorPreset("monitor-1");
        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::K7500);
    }

    SECTION("8200K") {
        backendPtr->getVcpResult.current = 0x07;
        const auto result = controller.getColorPreset("monitor-1");
        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::K8200);
    }

    SECTION("9300K") {
        backendPtr->getVcpResult.current = 0x08;
        const auto result = controller.getColorPreset("monitor-1");
        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::K9300);
    }

    SECTION("10000K") {
        backendPtr->getVcpResult.current = 0x09;
        const auto result = controller.getColorPreset("monitor-1");
        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::K10000);
    }

    SECTION("11500K") {
        backendPtr->getVcpResult.current = 0x0A;
        const auto result = controller.getColorPreset("monitor-1");
        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::K11500);
    }

    SECTION("User 1") {
        backendPtr->getVcpResult.current = 0x0B;
        const auto result = controller.getColorPreset("monitor-1");
        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::User1);
    }

    SECTION("User 2") {
        backendPtr->getVcpResult.current = 0x0C;
        const auto result = controller.getColorPreset("monitor-1");
        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::User2);
    }

    SECTION("User 3") {
        backendPtr->getVcpResult.current = 0x0D;
        const auto result = controller.getColorPreset("monitor-1");
        REQUIRE(result);
        REQUIRE(*result == vcpilot::ColorPreset::User3);
    }
}

TEST_CASE("MonitorController maps color presets to MCCS values") {
    auto backend = std::make_unique<FakeDdcBackend>();
    auto* backendPtr = backend.get();

    vcpilot::MonitorController controller(std::move(backend));

    SECTION("sRGB") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::SRgb));

        REQUIRE(backendPtr->lastSetVcpCode == 0x14);
        REQUIRE(backendPtr->lastSetVcpValue == 0x01);
    }

    SECTION("Display Native") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::DisplayNative));

        REQUIRE(backendPtr->lastSetVcpCode == 0x14);
        REQUIRE(backendPtr->lastSetVcpValue == 0x02);
    }

    SECTION("4000K") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::K4000));
        REQUIRE(backendPtr->lastSetVcpValue == 0x03);
    }

    SECTION("5000K") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::K5000));
        REQUIRE(backendPtr->lastSetVcpValue == 0x04);
    }

    SECTION("6500K") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::K6500));
        REQUIRE(backendPtr->lastSetVcpValue == 0x05);
    }

    SECTION("7500K") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::K7500));
        REQUIRE(backendPtr->lastSetVcpValue == 0x06);
    }

    SECTION("8200K") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::K8200));
        REQUIRE(backendPtr->lastSetVcpValue == 0x07);
    }

    SECTION("9300K") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::K9300));
        REQUIRE(backendPtr->lastSetVcpValue == 0x08);
    }

    SECTION("10000K") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::K10000));
        REQUIRE(backendPtr->lastSetVcpValue == 0x09);
    }

    SECTION("11500K") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::K11500));
        REQUIRE(backendPtr->lastSetVcpValue == 0x0A);
    }

    SECTION("User 1") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::User1));
        REQUIRE(backendPtr->lastSetVcpValue == 0x0B);
    }

    SECTION("User 2") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::User2));
        REQUIRE(backendPtr->lastSetVcpValue == 0x0C);
    }

    SECTION("User 3") {
        REQUIRE(controller.setColorPreset("monitor-1", vcpilot::ColorPreset::User3));
        REQUIRE(backendPtr->lastSetVcpValue == 0x0D);
    }
}

TEST_CASE("MonitorController rejects unknown color preset value") {
    auto backend = std::make_unique<FakeDdcBackend>();
    auto* backendPtr = backend.get();

    backendPtr->getVcpResult.current = 0x99;

    vcpilot::MonitorController controller(std::move(backend));

    const auto result = controller.getColorPreset("monitor-1");

    REQUIRE_FALSE(result);
    REQUIRE(result.error().code == vcpilot::ErrorCode::Unknown);
}

TEST_CASE("MonitorController rejects unknown power mode value") {
    auto backend = std::make_unique<FakeDdcBackend>();
    auto* backendPtr = backend.get();

    backendPtr->getVcpResult.current = 0x99;

    vcpilot::MonitorController controller(std::move(backend));

    const auto result = controller.getPowerMode("monitor-1");

    REQUIRE_FALSE(result);
    REQUIRE(result.error().code == vcpilot::ErrorCode::Unknown);
}