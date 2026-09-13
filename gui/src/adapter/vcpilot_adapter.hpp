#pragma once

#include <QObject>
#include <QVariantList>

#include <vcpilot/monitor_controller.hpp>

class VCPilotAdapter : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList monitors READ monitors NOTIFY monitorsChanged)

  public:
    explicit VCPilotAdapter(QObject* parent = nullptr);

    [[nodiscard]]
    QVariantList monitors() const;

    Q_INVOKABLE void refreshMonitors();

  signals:
    void monitorsChanged();

  private:
    vcpilot::MonitorController m_controller;
    QVariantList m_monitors;
};