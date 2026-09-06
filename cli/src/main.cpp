#include "vcpilot/logger.hpp"

int main() {
    
    vcpilot::log::initialize();

    VCPLOG_TRACE("VCPilot CLI started");

    return 0;
}