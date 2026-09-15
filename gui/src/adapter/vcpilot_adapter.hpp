#pragma once

#include <QObject>
#include <QThreadPool>
#include <QVariantList>

#include "adapter/monitor_state_manager.hpp"

#include <vcpilot/monitor_controller.hpp>

class VCPilotAdapter : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList monitors READ monitors NOTIFY monitorsChanged)

    Q_PROPERTY(QVariantList inputSources READ inputSources NOTIFY inputSourcesChanged)

    Q_PROPERTY(bool detecting READ detecting NOTIFY detectingChanged)

  public:
    explicit VCPilotAdapter(QObject* parent = nullptr);

    [[nodiscard]] QVariantList monitors() const;
    [[nodiscard]] QVariantList inputSources() const;
    [[nodiscard]] bool detecting() const;

    Q_INVOKABLE void refreshMonitors();

    Q_INVOKABLE void selectMonitor(const QString& monitorId);

    Q_INVOKABLE void setBrightness(const QString& monitorId, int value);

    Q_INVOKABLE void setContrast(const QString& monitorId, int value);

    Q_INVOKABLE void loadInputSources(const QString& monitorId);

    Q_INVOKABLE void setInputSource(const QString& monitorId, const QString& sourceKey);

    Q_INVOKABLE void setVolume(const QString& monitorId, int value);

    Q_INVOKABLE void setMute(const QString& monitorId, bool muted);

  signals:
    void monitorsChanged();
    void inputSourcesChanged();
    void detectingChanged();

  private:
    vcpilot::MonitorController m_controller;

    QThreadPool m_ddcThreadPool;
    MonitorStateManager m_monitorStateManager;

    QVariantList m_monitors;
    QVariantList m_inputSources;

    bool m_detecting{false};
};