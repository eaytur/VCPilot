#include "adapter/vcpilot_adapter.hpp"

#include <QFutureWatcher>
#include <QVariantMap>
#include <QtConcurrent>

#include <optional>
#include <string_view>

namespace {

QString inputSourceKey(vcpilot::InputSource source) {
    switch (source) {
    case vcpilot::InputSource::Vga1:
        return "vga1";

    case vcpilot::InputSource::Vga2:
        return "vga2";

    case vcpilot::InputSource::Dvi1:
        return "dvi1";

    case vcpilot::InputSource::Dvi2:
        return "dvi2";

    case vcpilot::InputSource::DisplayPort1:
        return "dp1";

    case vcpilot::InputSource::DisplayPort2:
        return "dp2";

    case vcpilot::InputSource::Hdmi1:
        return "hdmi1";

    case vcpilot::InputSource::Hdmi2:
        return "hdmi2";
    }

    return {};
}

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

    const std::string_view name = vcpilot::toString(source);

    return QString::fromUtf8(name.data(), static_cast<qsizetype>(name.size()));
}

std::optional<vcpilot::InputSource> inputSourceFromKey(const QString& key) {
    if (key == "vga1") {
        return vcpilot::InputSource::Vga1;
    }

    if (key == "vga2") {
        return vcpilot::InputSource::Vga2;
    }

    if (key == "dvi1") {
        return vcpilot::InputSource::Dvi1;
    }

    if (key == "dvi2") {
        return vcpilot::InputSource::Dvi2;
    }

    if (key == "dp1") {
        return vcpilot::InputSource::DisplayPort1;
    }

    if (key == "dp2") {
        return vcpilot::InputSource::DisplayPort2;
    }

    if (key == "hdmi1") {
        return vcpilot::InputSource::Hdmi1;
    }

    if (key == "hdmi2") {
        return vcpilot::InputSource::Hdmi2;
    }

    return std::nullopt;
}

} // namespace

VCPilotAdapter::VCPilotAdapter(QObject* parent)
    : QObject(parent), m_monitorStateManager(m_controller, m_ddcThreadPool, this) {

    m_ddcThreadPool.setMaxThreadCount(1);

    connect(&m_monitorStateManager, &MonitorStateManager::inputSourceChanged, this,
            [this](const QString& monitorId, const QString& inputSource) {
                for (qsizetype i = 0; i < m_monitors.size(); ++i) {

                    QVariantMap monitor = m_monitors[i].toMap();

                    if (monitor["id"].toString() != monitorId) {
                        continue;
                    }

                    if (monitor["currentInputSource"].toString() == inputSource) {
                        return;
                    }

                    monitor["currentInputSource"] = inputSource;

                    m_monitors[i] = monitor;

                    emit monitorsChanged();

                    return;
                }
            });
}

QVariantList VCPilotAdapter::monitors() const {
    return m_monitors;
}

void VCPilotAdapter::refreshMonitors() {
    if (m_detecting) {
        return;
    }

    m_detecting = true;
    emit detectingChanged();

    using ResultType = vcpilot::Result<std::vector<vcpilot::Monitor>>;

    auto* watcher = new QFutureWatcher<ResultType>(this);

    connect(watcher, &QFutureWatcher<ResultType>::finished, this, [this, watcher]() {
        const auto result = watcher->result();

        watcher->deleteLater();

        m_detecting = false;
        emit detectingChanged();

        if (!result) {
            return;
        }

        QVariantList monitors;
        monitors.reserve(static_cast<qsizetype>(result->size()));

        std::vector<std::string> controllableMonitorIds;
        controllableMonitorIds.reserve(result->size());

        for (const auto& monitor : *result) {
            const auto& info = monitor.info;

            QVariantMap item;

            item["id"] = QString::fromStdString(info.id);

            item["manufacturer"] = QString::fromStdString(info.manufacturer);

            item["model"] = QString::fromStdString(info.model);

            item["serial"] = QString::fromStdString(info.serial);

            item["primary"] = info.isPrimary;

            item["x"] = info.bounds.x;

            item["y"] = info.bounds.y;

            item["width"] = info.bounds.width;

            item["height"] = info.bounds.height;

            item["internalDisplay"] = info.isInternalDisplay;

            item["controllable"] =
                monitor.controlStatus == vcpilot::MonitorControlStatus::Supported;

            QString currentInputSource;

            for (const auto& existingMonitor : m_monitors) {
                const QVariantMap existing = existingMonitor.toMap();

                if (existing["id"].toString() == QString::fromStdString(info.id)) {

                    currentInputSource = existing["currentInputSource"].toString();

                    break;
                }
            }

            item["currentInputSource"] = currentInputSource;

            if (monitor.controlStatus == vcpilot::MonitorControlStatus::Supported) {

                controllableMonitorIds.push_back(info.id);
            }

            monitors.append(item);
        }

        if (m_monitors != monitors) {
            m_monitors = std::move(monitors);
            emit monitorsChanged();
        }

        m_monitorStateManager.setMonitors(std::move(controllableMonitorIds));

        m_monitorStateManager.start();
    });

    watcher->setFuture(
        QtConcurrent::run(&m_ddcThreadPool, [this]() { return m_controller.getMonitors(); }));
}
int VCPilotAdapter::brightness() const {
    return m_brightness;
}

void VCPilotAdapter::loadInputSources(const QString& monitorId) {
    if (monitorId.isEmpty()) {
        if (!m_inputSources.isEmpty()) {
            m_inputSources.clear();
            emit inputSourcesChanged();
        }

        return;
    }

    const auto supported = m_controller.getSupportedInputSources(monitorId.toStdString());

    if (!supported) {
        if (!m_inputSources.isEmpty()) {
            m_inputSources.clear();
            emit inputSourcesChanged();
        }

        return;
    }

    QVariantList sources;
    sources.reserve(static_cast<qsizetype>(supported->size()));

    for (const auto source : *supported) {
        QVariantMap item;

        item["key"] = inputSourceKey(source);
        item["name"] = inputSourceName(source);

        sources.append(item);
    }

    if (m_inputSources == sources) {
        return;
    }

    m_inputSources = std::move(sources);
    emit inputSourcesChanged();
}

void VCPilotAdapter::loadBrightness(const QString& monitorId) {

    if (monitorId.isEmpty()) {
        return;
    }

    const auto result = m_controller.getBrightness(monitorId.toStdString());

    if (!result) {
        return;
    }

    const int brightness = static_cast<int>(result->current);

    if (m_brightness == brightness) {
        return;
    }

    m_brightness = brightness;
    emit brightnessChanged();
}

void VCPilotAdapter::setBrightness(const QString& monitorId, int value) {

    if (monitorId.isEmpty()) {
        return;
    }

    const auto result =
        m_controller.setBrightness(monitorId.toStdString(), static_cast<std::uint16_t>(value));

    if (!result) {
        return;
    }

    if (m_brightness == value) {
        return;
    }

    m_brightness = value;
    emit brightnessChanged();
}

QVariantList VCPilotAdapter::inputSources() const {
    return m_inputSources;
}

void VCPilotAdapter::setInputSource(const QString& monitorId, const QString& sourceKey) {

    if (monitorId.isEmpty()) {
        return;
    }

    const auto source = inputSourceFromKey(sourceKey);

    if (!source) {
        return;
    }

    const auto result = m_controller.setInputSource(monitorId.toStdString(), *source);

    if (!result) {
        return;
    }
}

bool VCPilotAdapter::detecting() const {
    return m_detecting;
}
