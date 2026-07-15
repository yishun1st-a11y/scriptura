#ifndef PLUGINHOSTPROTOCOL_H
#define PLUGINHOSTPROTOCOL_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

/**
 * @file pluginhostprotocol.h
 * @brief Defines the JSON protocol between Scriptura IDE and pluginhost
 */

class PluginHostProtocol
{
public:
    enum class Command : int {
        Load = 1,
        Initialize = 2,
        Shutdown = 3,
        Unload = 4,
        Call = 5,
        Ping = 6
    };

    static QJsonObject loadCommand(const QString &pluginId, const QString &libraryPath);
    static QJsonObject initializeCommand(const QString &pluginId, const QJsonObject &context);
    static QJsonObject shutdownCommand(const QString &pluginId);
    static QJsonObject unloadCommand(const QString &pluginId);
    static QJsonObject callCommand(const QString &pluginId, const QString &method, const QJsonArray &args);
    static QJsonObject pingCommand();

    static Command commandType(const QJsonObject &obj);
    static QString pluginId(const QJsonObject &obj);
    static QString method(const QJsonObject &obj);
    static QJsonArray arguments(const QJsonObject &obj);
    static int requestId(const QJsonObject &obj);

    static QJsonObject okResponse(int requestId, const QJsonValue &result = QJsonValue{});
    static QJsonObject errorResponse(int requestId, const QString &error);
    static QJsonObject event(const QString &event, const QJsonObject &data);
};

#endif // PLUGINHOSTPROTOCOL_H
