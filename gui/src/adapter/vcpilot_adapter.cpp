#include "adapter/vcpilot_adapter.hpp"

#include "vcpilot/mccs_catalog.hpp"

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

    connect(&m_monitorStateManager, &MonitorStateManager::brightnessChanged, this,
            [this](const QString& monitorId, int current, int maximum) {
                for (qsizetype i = 0; i < m_monitors.size(); ++i) {

                    QVariantMap monitor = m_monitors[i].toMap();

                    if (monitor["id"].toString() != monitorId) {
                        continue;
                    }

                    if (monitor["brightness"].toInt() == current &&
                        monitor["brightnessMaximum"].toInt() == maximum) {
                        return;
                    }

                    monitor["brightness"] = current;
                    monitor["brightnessMaximum"] = maximum;

                    m_monitors[i] = monitor;

                    emit monitorsChanged();
                    return;
                }
            });

    connect(&m_monitorStateManager, &MonitorStateManager::contrastChanged, this,
            [this](const QString& monitorId, int current, int maximum) {
                for (qsizetype i = 0; i < m_monitors.size(); ++i) {

                    QVariantMap monitor = m_monitors[i].toMap();

                    if (monitor["id"].toString() != monitorId) {
                        continue;
                    }

                    if (monitor["contrast"].toInt() == current &&
                        monitor["contrastMaximum"].toInt() == maximum) {
                        return;
                    }

                    monitor["contrast"] = current;
                    monitor["contrastMaximum"] = maximum;

                    m_monitors[i] = monitor;

                    emit monitorsChanged();
                    return;
                }
            });

    connect(&m_monitorStateManager, &MonitorStateManager::volumeChanged, this,
            [this](const QString& monitorId, int current, int maximum) {
                for (qsizetype i = 0; i < m_monitors.size(); ++i) {

                    QVariantMap monitor = m_monitors[i].toMap();

                    if (monitor["id"].toString() != monitorId) {
                        continue;
                    }

                    if (monitor["volume"].toInt() == current &&
                        monitor["volumeMaximum"].toInt() == maximum) {
                        return;
                    }

                    monitor["volume"] = current;
                    monitor["volumeMaximum"] = maximum;

                    m_monitors[i] = monitor;

                    emit monitorsChanged();
                    return;
                }
            });

    connect(&m_monitorStateManager, &MonitorStateManager::muteChanged, this,
            [this](const QString& monitorId, bool muted) {
                for (qsizetype i = 0; i < m_monitors.size(); ++i) {

                    QVariantMap monitor = m_monitors[i].toMap();

                    if (monitor["id"].toString() != monitorId) {
                        continue;
                    }

                    if (monitor["muted"].toBool() == muted) {
                        return;
                    }

                    monitor["muted"] = muted;

                    m_monitors[i] = monitor;

                    emit monitorsChanged();
                    return;
                }
            });
}

QVariantList VCPilotAdapter::monitors() const {
    return m_monitors;
}

QVariantList VCPilotAdapter::inputSources() const {
    return m_inputSources;
}

bool VCPilotAdapter::detecting() const {
    return m_detecting;
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

            QVariantList vcpFeatures;
            QString mccsVersion;

            if (monitor.capabilities) {
                qDebug() << "  MCCS:" << QString::fromStdString(monitor.capabilities->mccsVersion);

                qDebug() << "  VCP feature count:" << monitor.capabilities->vcpFeatures.size();

                mccsVersion = QString::fromStdString(monitor.capabilities->mccsVersion);

                for (const auto& capability : monitor.capabilities->vcpFeatures) {

                    QVariantMap feature;

                    feature["code"] = static_cast<int>(capability.code);

                    const auto* descriptor = vcpilot::findVcpFeatureDescriptor(capability.code);

                    if (descriptor) {
                        feature["name"] =
                            QString::fromUtf8(descriptor->name.data(),
                                              static_cast<qsizetype>(descriptor->name.size()));

                        feature["access"] = QString::fromStdString(
                            std::string(vcpilot::toString(descriptor->access)));

                        feature["type"] = QString::fromStdString(
                            std::string(vcpilot::toString(descriptor->type)));
                    } else {
                        feature["name"] = "Unknown VCP Feature";
                        feature["access"] = "Unknown";
                        feature["type"] = "Unknown";
                    }

                    QVariantList values;

                    for (const auto value : capability.values) {
                        values.append(static_cast<int>(value));
                    }

                    feature["values"] = values;

                    vcpFeatures.append(feature);
                }
            }

            item["mccsVersion"] = mccsVersion;

            item["vcpFeatures"] = vcpFeatures;

            QString currentInputSource;

            int brightness = 0;
            int brightnessMaximum = 0;

            int contrast = 0;
            int contrastMaximum = 0;

            int volume = 0;
            int volumeMaximum = 0;

            bool muted = false;

            for (const auto& existingMonitor : m_monitors) {
                const QVariantMap existing = existingMonitor.toMap();

                if (existing["id"].toString() != QString::fromStdString(info.id)) {
                    continue;
                }

                currentInputSource = existing["currentInputSource"].toString();

                brightness = existing["brightness"].toInt();
                brightnessMaximum = existing["brightnessMaximum"].toInt();

                contrast = existing["contrast"].toInt();
                contrastMaximum = existing["contrastMaximum"].toInt();

                volume = existing["volume"].toInt();
                volumeMaximum = existing["volumeMaximum"].toInt();

                muted = existing["muted"].toBool();

                break;
            }

            item["currentInputSource"] = currentInputSource;

            item["brightness"] = brightness;
            item["brightnessMaximum"] = brightnessMaximum;

            item["contrast"] = contrast;
            item["contrastMaximum"] = contrastMaximum;

            item["volume"] = volume;
            item["volumeMaximum"] = volumeMaximum;

            item["muted"] = muted;

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

void VCPilotAdapter::setBrightness(const QString& monitorId, int value) {

    if (monitorId.isEmpty()) {
        return;
    }

    m_controller.setBrightness(monitorId.toStdString(), static_cast<std::uint16_t>(value));
}

void VCPilotAdapter::setContrast(const QString& monitorId, int value) {

    if (monitorId.isEmpty()) {
        return;
    }

    m_controller.setContrast(monitorId.toStdString(), static_cast<std::uint16_t>(value));
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

void VCPilotAdapter::setInputSource(const QString& monitorId, const QString& sourceKey) {

    if (monitorId.isEmpty()) {
        return;
    }

    const auto source = inputSourceFromKey(sourceKey);

    if (!source) {
        return;
    }

    m_controller.setInputSource(monitorId.toStdString(), *source);
}

void VCPilotAdapter::setVolume(const QString& monitorId, int value) {

    if (monitorId.isEmpty()) {
        return;
    }

    const auto result =
        m_controller.setVolume(monitorId.toStdString(), static_cast<std::uint16_t>(value));

    if (!result) {
        return;
    }
}

void VCPilotAdapter::setMute(const QString& monitorId, bool muted) {

    if (monitorId.isEmpty()) {
        return;
    }

    const auto result = m_controller.setMute(monitorId.toStdString(), muted);

    if (!result) {
        return;
    }

    for (qsizetype i = 0; i < m_monitors.size(); ++i) {
        QVariantMap monitor = m_monitors[i].toMap();

        if (monitor["id"].toString() != monitorId) {
            continue;
        }

        monitor["muted"] = muted;
        m_monitors[i] = monitor;

        emit monitorsChanged();
        return;
    }
}

QVariantMap VCPilotAdapter::getVcp(const QString& monitorId, int code) {

    QVariantMap response;

    if (monitorId.isEmpty()) {
        response["success"] = false;
        response["error"] = "No monitor selected";
        return response;
    }

    if (code < 0 || code > 0xFF) {
        response["success"] = false;
        response["error"] = "VCP code must be between 0x00 and 0xFF";
        return response;
    }

    const auto result =
        m_controller.getVcp(monitorId.toStdString(), static_cast<std::uint8_t>(code));

    if (!result) {
        response["success"] = false;
        response["error"] = QString::fromStdString(result.error().message);

        return response;
    }

    response["success"] = true;
    response["current"] = static_cast<int>(result->current);
    response["maximum"] = static_cast<int>(result->maximum);

    return response;
}

QVariantMap VCPilotAdapter::setVcp(const QString& monitorId, int code, int value) {

    QVariantMap response;

    if (monitorId.isEmpty()) {
        response["success"] = false;
        response["error"] = "No monitor selected";
        return response;
    }

    if (code < 0 || code > 0xFF) {
        response["success"] = false;
        response["error"] = "VCP code must be between 0x00 and 0xFF";
        return response;
    }

    if (value < 0 || value > 0xFFFF) {
        response["success"] = false;
        response["error"] = "VCP value must be between 0x0000 and 0xFFFF";

        return response;
    }

    const auto result =
        m_controller.setVcp(monitorId.toStdString(), static_cast<std::uint8_t>(code),
                            static_cast<std::uint16_t>(value));

    if (!result) {
        response["success"] = false;
        response["error"] = QString::fromStdString(result.error().message);

        return response;
    }

    response["success"] = true;

    return response;
}
