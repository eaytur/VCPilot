#include "adapter/monitor_state_manager.hpp"

#include <QFutureWatcher>
#include <QtConcurrent>

#include <optional>
#include <utility>
#include <vector>

namespace {

QString inputSourceName(vcpilot::InputSource source) {
    switch (source) {
    case vcpilot::InputSource::Vga1:
        return "VGA 1";

    case vcpilot::InputSource::Vga2:
        return "VGA 2";

    case vcpilot::InputSource::Dvi1:
        return "DVI 1";

    case vcpilot::InputSource::Dvi2:
        return "DVI 2";

    case vcpilot::InputSource::DisplayPort1:
        return "DisplayPort 1";

    case vcpilot::InputSource::DisplayPort2:
        return "DisplayPort 2";

    case vcpilot::InputSource::Hdmi1:
        return "HDMI 1";

    case vcpilot::InputSource::Hdmi2:
        return "HDMI 2";
    }

    return {};
}

struct StateResult {
    QString monitorId;
    std::optional<vcpilot::InputSource> inputSource;
};

} // namespace

MonitorStateManager::MonitorStateManager(vcpilot::MonitorController& controller,
                                         QThreadPool& threadPool, QObject* parent)
    : QObject(parent), m_controller(controller), m_threadPool(threadPool) {

    m_timer.setInterval(1500);

    connect(&m_timer, &QTimer::timeout, this, &MonitorStateManager::poll);
}

void MonitorStateManager::setMonitors(std::vector<std::string> monitorIds) {

    m_monitorIds = std::move(monitorIds);

    poll();
}

void MonitorStateManager::start() {
    if (m_timer.isActive()) {
        return;
    }

    m_timer.start();
    poll();
}

void MonitorStateManager::stop() {
    m_timer.stop();
}

void MonitorStateManager::poll() {

    if (m_polling || m_monitorIds.empty()) {
        return;
    }

    m_polling = true;

    using ResultList = std::vector<StateResult>;

    auto* watcher = new QFutureWatcher<ResultList>(this);

    connect(watcher, &QFutureWatcher<ResultList>::finished, this, [this, watcher]() {
        const auto results = watcher->result();

        watcher->deleteLater();

        m_polling = false;

        for (const auto& result : results) {

            if (!result.inputSource) {
                continue;
            }

            const std::string id = result.monitorId.toStdString();

            const auto previous = m_inputSources.find(id);

            if (previous != m_inputSources.end() && previous->second == *result.inputSource) {
                continue;
            }

            m_inputSources[id] = *result.inputSource;

            emit inputSourceChanged(result.monitorId, inputSourceName(*result.inputSource));
        }
    });

    const auto monitorIds = m_monitorIds;

    watcher->setFuture(QtConcurrent::run(&m_threadPool, [this, monitorIds]() {
        ResultList results;
        results.reserve(monitorIds.size());

        for (const auto& monitorId : monitorIds) {

            StateResult result;
            result.monitorId = QString::fromStdString(monitorId);

            auto state = m_controller.getMonitorState(monitorId);

            if (state && state->inputSource) {
                result.inputSource = state->inputSource;
            }

            results.push_back(std::move(result));
        }

        return results;
    }));
}