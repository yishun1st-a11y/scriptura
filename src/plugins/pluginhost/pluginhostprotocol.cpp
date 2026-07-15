#include "pluginhostprotocol.h"

QJsonObject PluginHostProtocol::loadCommand(const QString &pluginId, const QString &libraryPath)
{
    QJsonObject obj;
    obj["cmd"] = static_cast<int>(Command::Load);
    obj["id"] = 1;
    obj["pluginId"] = pluginId;
    obj["libraryPath"] = libraryPath;
    return obj;
}

QJsonObject PluginHostProtocol::initializeCommand(const QString &pluginId, const QJsonObject &context)
{
    QJsonObject obj;
    obj["cmd"] = static_cast<int>(Command::Initialize);
    obj["id"] = 2;
    obj["pluginId"] = pluginId;
    obj["context"] = context;
    return obj;
}

QJsonObject PluginHostProtocol::shutdownCommand(const QString &pluginId)
{
    QJsonObject obj;
    obj["cmd"] = static_cast<int>(Command::Shutdown);
    obj["id"] = 3;
    obj["pluginId"] = pluginId;
    return obj;
}

QJsonObject PluginHostProtocol::unloadCommand(const QString &pluginId)
{
    QJsonObject obj;
    obj["cmd"] = static_cast<int>(Command::Unload);
    obj["id"] = 4;
    obj["pluginId"] = pluginId;
    return obj;
}

QJsonObject PluginHostProtocol::callCommand(const QString &pluginId, const QString &method, const QJsonArray &args)
{
    QJsonObject obj;
    obj["cmd"] = static_cast<int>(Command::Call);
    obj["id"] = 5;
    obj["pluginId"] = pluginId;
    obj["method"] = method;
    obj["args"] = args;
    return obj;
}

QJsonObject PluginHostProtocol::pingCommand()
{
    QJsonObject obj;
    obj["cmd"] = static_cast<int>(Command::Ping);
    obj["id"] = 6;
    return obj;
}

PluginHostProtocol::Command PluginHostProtocol::commandType(const QJsonObject &obj)
{
    int cmd = obj.value("cmd").toInt(0);
    return static_cast<Command>(cmd);
}

QString PluginHostProtocol::pluginId(const QJsonObject &obj)
{
    return obj.value("pluginId").toString();
}

QString PluginHostProtocol::method(const QJsonObject &obj)
{
    return obj.value("method").toString();
}

QJsonArray PluginHostProtocol::arguments(const QJsonObject &obj)
{
    return obj.value("args").toArray();
}

int PluginHostProtocol::requestId(const QJsonObject &obj)
{
    return obj.value("id").toInt(0);
}

QJsonObject PluginHostProtocol::okResponse(int requestId, const QJsonValue &result)
{
    QJsonObject obj;
    obj["id"] = requestId;
    obj["ok"] = true;
    obj["result"] = result;
    return obj;
}

QJsonObject PluginHostProtocol::errorResponse(int requestId, const QString &error)
{
    QJsonObject obj;
    obj["id"] = requestId;
    obj["ok"] = false;
    obj["error"] = error;
    return obj;
}

QJsonObject PluginHostProtocol::event(const QString &event, const QJsonObject &data)
{
    QJsonObject obj;
    obj["event"] = event;
    obj["data"] = data;
    return obj;
}
