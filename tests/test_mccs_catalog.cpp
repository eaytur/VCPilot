#include "vcpilot/mccs_catalog.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("MCCS catalog returns metadata for known VCP features") {
    const auto* brightness = vcpilot::findVcpFeatureDescriptor(0x10);

    REQUIRE(brightness != nullptr);
    REQUIRE(brightness->code == 0x10);
    REQUIRE(brightness->name == "Brightness");
    REQUIRE(brightness->access == vcpilot::VcpAccess::ReadWrite);
    REQUIRE(brightness->type == vcpilot::VcpFeatureType::Continuous);
}

TEST_CASE("MCCS catalog returns metadata for input source") {
    const auto* inputSource = vcpilot::findVcpFeatureDescriptor(0x60);

    REQUIRE(inputSource != nullptr);
    REQUIRE(inputSource->name == "Input Source");
    REQUIRE(inputSource->access == vcpilot::VcpAccess::ReadWrite);
    REQUIRE(inputSource->type == vcpilot::VcpFeatureType::NonContinuousSimple);
}

TEST_CASE("MCCS catalog returns null for unknown VCP feature") {
    const auto* feature = vcpilot::findVcpFeatureDescriptor(0xFF);

    REQUIRE(feature == nullptr);
}

TEST_CASE("MCCS catalog resolves discrete VCP values") {
    SECTION("HDMI 1") {
        const auto* value = vcpilot::findVcpValueDescriptor(0x60, 0x11);

        REQUIRE(value != nullptr);
        REQUIRE(value->name == "HDMI 1");
    }

    SECTION("Power on") {
        const auto* value = vcpilot::findVcpValueDescriptor(0xD6, 0x01);

        REQUIRE(value != nullptr);
        REQUIRE(value->name == "On");
    }

    SECTION("Unknown value") {
        const auto* value = vcpilot::findVcpValueDescriptor(0x60, 0xFF);

        REQUIRE(value == nullptr);
    }
}