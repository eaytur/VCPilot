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

        return vcpilot::VcpValue{};
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