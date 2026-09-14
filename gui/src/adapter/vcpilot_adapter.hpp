#pragma once

#include <QObject>
#include <QThreadPool>
#include <QVariantList>

#include <vcpilot/monitor_controller.hpp>

class VCPilotAdapter : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList monitors READ monitors NOTIFY monitorsChanged)
    Q_PROPERTY(int brightness READ brightness NOTIFY brightnessChanged)
    Q_PROPERTY(QVariantList inputSources READ inputSources NOTIFY inputSourcesChanged)
    Q_PROPERTY(QString currentInputSource READ currentInputSource NOTIFY currentInputSourceChanged)
    Q_PROPERTY(bool detecting READ detecting NOTIFY detectingChanged)
    Q_PROPERTY(bool inputSourceLoading READ inputSourceLoading NOTIFY inputSourceLoadingChanged)

  public:
    explicit VCPilotAdapter(QObject* parent = nullptr);

    [[nodiscard]] QVariantList monitors() const;
    [[nodiscard]] int brightness() const;
    [[nodiscard]] QVariantList inputSources() const;
    [[nodiscard]] QString currentInputSource() const;
    [[nodiscard]] bool detecting() const;
    [[nodiscard]] bool inputSourceLoading() const;

    Q_INVOKABLE void refreshMonitors();

    Q_INVOKABLE void loadBrightness(const QString& monitorId);
    Q_INVOKABLE void setBrightness(const QString& monitorId, int value);
    Q_INVOKABLE void loadInputControl(const QString& monitorId);
    Q_INVOKABLE void setInputSource(const QString& monitorId, const QString& sourceKey);

  signals:
    void monitorsChanged();
    void brightnessChanged();
    void inputSourcesChanged();
    void currentInputSourceChanged();
    void detectingChanged();
    void inputSourceLoadingChanged();

  private:
    vcpilot::MonitorController m_controller;
    QVariantList m_monitors;
    int m_brightness{0};
    QVariantList m_inputSources;
    QString m_currentInputSource;
    bool m_detecting{false};
    QThreadPool m_ddcThreadPool;
    bool m_inputSourceLoading{false};
    quint64 m_inputSourceLoadGeneration{0};
};