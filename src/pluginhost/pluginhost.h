#ifndef PLUGINHOST_H
#define PLUGINHOST_H

#include <QObject>
#include <QProcess>
#include <QPluginLoader>
#include "lengthprefixedframer.h"

class PluginHost : public QObject
{
    Q_OBJECT
public:
    explicit PluginHost(QObject *parent = nullptr);
    bool loadPlugin(const QString &pluginDir);
    void run();
    void shutdown();

signals:
    void started();
    void stopped();
    void error(const QString &message);

private slots:
    void onStdinReadyRead();
    void onPluginCrashed(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QJsonObject handleRequest(const QJsonObject &request);
    QJsonObject handlePing(const QJsonObject &params);
    QJsonObject handleShutdown(const QJsonObject &params);

    QPluginLoader *m_loader = nullptr;
    QObject *m_pluginInstance = nullptr;
    LengthPrefixedFramer *m_framer = nullptr;
    bool m_running = false;
};

#endif // PLUGINHOST_H
