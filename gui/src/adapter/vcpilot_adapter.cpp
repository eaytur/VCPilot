#include "adapter/vcpilot_adapter.hpp"

#include <QVariantMap>

VCPilotAdapter::VCPilotAdapter(QObject* parent) : QObject(parent) {}

QVariantList VCPilotAdapter::monitors() const {
    return m_monitors;
}

void VCPilotAdapter::refreshMonitors() {
    const auto result = m_controller.getMonitorInfos();

    if (!result) {
        return;
    }

    QVariantList monitors;

    for (const auto& monitorInfo : *result) {
        QVariantMap monitor;

        monitor["id"] = QString::fromStdString(monitorInfo.id);
        monitor["manufacturer"] = QString::fromStdString(monitorInfo.manufacturer);
        monitor["model"] = QString::fromStdString(monitorInfo.model);
        monitor["serial"] = QString::fromStdString(monitorInfo.serial);

        monitor["primary"] = monitorInfo.isPrimary;

        monitor["x"] = monitorInfo.bounds.x;
        monitor["y"] = monitorInfo.bounds.y;
        monitor["width"] = monitorInfo.bounds.width;
        monitor["height"] = monitorInfo.bounds.height;
        monitor["internalDisplay"] = monitorInfo.isInternalDisplay;

        monitors.append(monitor);
    }

    m_monitors = std::move(monitors);

    emit monitorsChanged();
}