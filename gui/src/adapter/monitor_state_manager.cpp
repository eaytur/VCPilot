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
    std::optional<vcpilot::VcpValue> brightness;
    std::optional<vcpilot::VcpValue> contrast;
    std::optional<vcpilot::VcpValue> volume;
    std::optional<bool> muted;
};

} // namespace

MonitorStateManager::MonitorStateManager(vcpilot::MonitorController& controller,
                                         QThreadPool& threadPool, QObject* parent)
    : QObject(parent), m_controller(controller), m_threadPool(threadPool) {

    m_timer.setInterval(2000);

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

            const std::string id = result.monitorId.toStdString();

            if (result.inputSource) {

                const auto previous = m_inputSources.find(id);

                if (previous == m_inputSources.end() || previous->second != *result.inputSource) {

                    m_inputSources[id] = *result.inputSource;

                    emit inputSourceChanged(result.monitorId, inputSourceName(*result.inputSource));
                }
            }

            if (result.brightness) {

                const auto previous = m_brightnessValues.find(id);

                const auto& value = *result.brightness;

                if (previous == m_brightnessValues.end() ||
                    previous->second.current != value.current ||
                    previous->second.maximum != value.maximum) {

                    m_brightnessValues[id] = value;

                    emit brightnessChanged(result.monitorId, value.current, value.maximum);
                }
            }

            if (result.contrast) {

                const auto previous = m_contrastValues.find(id);

                const auto& value = *result.contrast;

                if (previous == m_contrastValues.end() ||
                    previous->second.current != value.current ||
                    previous->second.maximum != value.maximum) {

                    m_contrastValues[id] = value;

                    emit contrastChanged(result.monitorId, value.current, value.maximum);
                }
            }

            if (result.volume) {

                const auto previous = m_volumeValues.find(id);

                const auto& value = *result.volume;

                if (previous == m_volumeValues.end() || previous->second.current != value.current ||
                    previous->second.maximum != value.maximum) {

                    m_volumeValues[id] = value;

                    emit volumeChanged(result.monitorId, value.current, value.maximum);
                }
            }

            if (result.muted) {

                const auto previous = m_muteValues.find(id);

                const bool value = *result.muted;

                if (previous == m_muteValues.end() || previous->second != value) {

                    m_muteValues[id] = value;

                    emit muteChanged(result.monitorId, value);
                }
            }
        }
    });

    const auto monitorIds = m_monitorIds;

    watcher->setFuture(QtConcurrent::run(&m_threadPool, [this, monitorIds]() {
        ResultList results;

        results.reserve(monitorIds.size());

        for (const auto& monitorId : monitorIds) {

            StateResult result;

            result.monitorId = QString::fromStdString(monitorId);

            if (auto state = m_controller.getMonitorState(monitorId)) {

                result.inputSource = state->inputSource;

                result.brightness = state->brightness;

                result.contrast = state->contrast;

                result.volume = state->volume;

                result.muted = state->muted;
            }

            results.push_back(std::move(result));
        }

        return results;
    }));
}