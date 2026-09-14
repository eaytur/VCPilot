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

    void start();
    void stop();

  signals:
    void inputSourceChanged(const QString& monitorId, const QString& source);

  private:
    void poll();

    vcpilot::MonitorController& m_controller;
    QThreadPool& m_threadPool;

    QTimer m_timer;

    std::vector<std::string> m_monitorIds;

    bool m_polling{false};
    std::unordered_map<std::string, vcpilot::InputSource> m_inputSources;
};