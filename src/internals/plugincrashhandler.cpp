#include "plugincrashhandler.h"
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>

PluginCrashHandler::PluginCrashHandler(QObject* parent)
    : QObject(parent)
    , m_crashLogPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/plugin_crashes.log")
{
    QDir().mkpath(QFileInfo(m_crashLogPath).absolutePath());
}

PluginCrashHandler::~PluginCrashHandler()
{
    qDeleteAll(m_pluginProcesses);
    m_pluginProcesses.clear();
}

void PluginCrashHandler::registerPluginProcess(const QString& pluginId, QProcess* process)
{
    if (!process) return;

    if (m_pluginProcesses.contains(pluginId)) {
        delete m_pluginProcesses.take(pluginId);
    }

    m_pluginProcesses[pluginId] = process;

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, pluginId](int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitStatus);

        if (exitCode != 0) {
            handleCrash(pluginId);
        }
    });
}

void PluginCrashHandler::handleCrash(const QString& pluginId)
{
    CrashInfo info;
    info.pluginId = pluginId;
    info.timestamp = QDateTime::currentDateTime();
    info.errorType = "Process crashed";
    info.stackTrace = QString();
    info.autoDisabled = true;

    m_crashHistory.prepend(info);
    if (m_crashHistory.size() > 100) {
        m_crashHistory.removeLast();
    }

    QString logEntry = QString("[%1] Plugin crashed: %2\n")
                           .arg(info.timestamp.toString(Qt::ISODate))
                           .arg(pluginId);
    QFile logFile(m_crashLogPath);
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << logEntry;
        logFile.close();
    }

    disablePlugin(pluginId);

    emit pluginCrashed(pluginId, info);

    qWarning() << "Plugin crashed:" << pluginId << "at" << info.timestamp;
}

void PluginCrashHandler::disablePlugin(const QString& pluginId)
{
    m_disabledPlugins[pluginId] = true;

    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/disabled_plugins.ini";
    QDir().mkpath(QFileInfo(settingsPath).absolutePath());
    QSettings settings(settingsPath, QSettings::IniFormat);
    settings.beginWriteArray("disabled");
    int index = 0;
    for (auto it = m_disabledPlugins.constBegin(); it != m_disabledPlugins.constEnd(); ++it) {
        settings.setArrayIndex(index++);
        settings.setValue("id", it.key());
    }
    settings.endArray();
}

QList<CrashInfo> PluginCrashHandler::recentCrashes(int limit)
{
    if (limit <= 0 || limit >= m_crashHistory.size()) {
        return m_crashHistory;
    }
    return m_crashHistory.mid(0, limit);
}

bool PluginCrashHandler::isPluginDisabled(const QString& pluginId) const
{
    return m_disabledPlugins.value(pluginId, false);
}

void PluginCrashHandler::enablePlugin(const QString& pluginId)
{
    m_disabledPlugins.remove(pluginId);

    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/disabled_plugins.ini";
    QDir().mkpath(QFileInfo(settingsPath).absolutePath());
    QSettings settings(settingsPath, QSettings::IniFormat);
    settings.beginWriteArray("disabled");
    int index = 0;
    for (auto it = m_disabledPlugins.constBegin(); it != m_disabledPlugins.constEnd(); ++it) {
        settings.setArrayIndex(index++);
        settings.setValue("id", it.key());
    }
    settings.endArray();
}
