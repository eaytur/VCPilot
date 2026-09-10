#include "vcpilot/monitor_capabilities.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Capabilities parser extracts MCCS version") {
    const auto capabilities =
        vcpilot::parseCapabilitiesString("(vcp(10 12 60(0F 11))mccs_ver(2.0))");

    REQUIRE(capabilities.mccsVersion == "2.0");
}

TEST_CASE("Capabilities parser extracts VCP feature codes") {
    const auto capabilities = vcpilot::parseCapabilitiesString("(vcp(10 12 60)mccs_ver(2.0))");

    REQUIRE(capabilities.vcpFeatures.size() == 3);

    REQUIRE(capabilities.vcpFeatures[0].code == 0x10);
    REQUIRE(capabilities.vcpFeatures[1].code == 0x12);
    REQUIRE(capabilities.vcpFeatures[2].code == 0x60);
}

TEST_CASE("Capabilities parser extracts discrete VCP values") {
    const auto capabilities =
        vcpilot::parseCapabilitiesString("(vcp(10 60(0F 11) D6(01 02 04))mccs_ver(2.0))");

    REQUIRE(capabilities.vcpFeatures.size() == 3);

    const auto& inputSource = capabilities.vcpFeatures[1];

    REQUIRE(inputSource.code == 0x60);
    REQUIRE(inputSource.values.size() == 2);
    REQUIRE(inputSource.values[0] == 0x0F);
    REQUIRE(inputSource.values[1] == 0x11);

    const auto& powerMode = capabilities.vcpFeatures[2];

    REQUIRE(powerMode.code == 0xD6);
    REQUIRE(powerMode.values.size() == 3);
    REQUIRE(powerMode.values[0] == 0x01);
    REQUIRE(powerMode.values[1] == 0x02);
    REQUIRE(powerMode.values[2] == 0x04);
}

TEST_CASE("Capabilities parser preserves unknown VCP features") {
    const auto capabilities = vcpilot::parseCapabilitiesString("(vcp(10 FF)mccs_ver(2.0))");

    REQUIRE(capabilities.vcpFeatures.size() == 2);
    REQUIRE(capabilities.vcpFeatures[1].code == 0xFF);
}

TEST_CASE("Capabilities parser handles missing sections") {
    SECTION("Missing VCP section") {
        const auto capabilities = vcpilot::parseCapabilitiesString("(prot(monitor)mccs_ver(2.0))");

        REQUIRE(capabilities.vcpFeatures.empty());
        REQUIRE(capabilities.mccsVersion == "2.0");
    }

    SECTION("Missing MCCS version") {
        const auto capabilities = vcpilot::parseCapabilitiesString("(vcp(10 12))");

        REQUIRE(capabilities.mccsVersion.empty());
        REQUIRE(capabilities.vcpFeatures.size() == 2);
    }
}

TEST_CASE("Capabilities parser handles empty input") {
    const auto capabilities = vcpilot::parseCapabilitiesString("");

    REQUIRE(capabilities.vcpFeatures.empty());
    REQUIRE(capabilities.mccsVersion.empty());
}

TEST_CASE("Capabilities parser ignores invalid VCP tokens") {
    const auto capabilities =
        vcpilot::parseCapabilitiesString("(vcp(10 ZZ 60(0F 11))mccs_ver(2.0))");

    REQUIRE(capabilities.vcpFeatures.size() == 2);
    REQUIRE(capabilities.vcpFeatures[0].code == 0x10);
    REQUIRE(capabilities.vcpFeatures[1].code == 0x60);
}

TEST_CASE("Capabilities parser handles unclosed VCP section") {
    const auto capabilities = vcpilot::parseCapabilitiesString("(vcp(10 60(0F 11)");

    REQUIRE(capabilities.vcpFeatures.empty());
}

TEST_CASE("Capabilities parser ignores VCP codes outside byte range") {
    const auto capabilities =
        vcpilot::parseCapabilitiesString("(vcp(10 1FF 60(0F 11))mccs_ver(2.0))");

    REQUIRE(capabilities.vcpFeatures.size() == 2);
    REQUIRE(capabilities.vcpFeatures[0].code == 0x10);
    REQUIRE(capabilities.vcpFeatures[1].code == 0x60);
}

TEST_CASE("Out-of-range VCP code does not expose nested values as features") {

    const auto result = vcpilot::parseCapabilitiesString("(vcp(100(01 02) 10 12))");

    REQUIRE(result.vcpFeatures.size() == 2);
    CHECK(result.vcpFeatures[0].code == 0x10);
    CHECK(result.vcpFeatures[1].code == 0x12);
}