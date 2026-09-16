#pragma once

#include <QObject>
#include <QThreadPool>
#include <QTimer>

#include <vcpilot/monitor_controller.hpp>

#include <string>
#include <unordered_map>
#include <vector>

class MonitorStateManager : public QObject {
    Q_OBJECT

  public:
    explicit MonitorStateManager(vcpilot::MonitorController& controller, QThreadPool& threadPool,
                                 QObject* parent = nullptr);

    void setMonitors(std::vector<std::string> monitorIds);
    void setSelectedMonitor(const std::string& monitorId);

    void start();
    void stop();

  signals:
    void inputSourceChanged(const QString& monitorId, const QString& source);
    void brightnessChanged(const QString& monitorId, int current, int maximum);
    void contrastChanged(const QString& monitorId, int current, int maximum);
    void volumeChanged(const QString& monitorId, int current, int maximum);
    void muteChanged(const QString& monitorId, bool muted);

  private:
    void poll();

    vcpilot::MonitorController& m_controller;
    QThreadPool& m_threadPool;

    QTimer m_timer;

    std::vector<std::string> m_monitorIds;

    std::unordered_map<std::string, vcpilot::InputSource> m_inputSources;
    std::unordered_map<std::string, vcpilot::VcpValue> m_brightnessValues;
    std::unordered_map<std::string, vcpilot::VcpValue> m_contrastValues;
    std::unordered_map<std::string, vcpilot::VcpValue> m_volumeValues;
    std::unordered_map<std::string, bool> m_muteValues;
    bool m_polling{false};
};