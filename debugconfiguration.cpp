#include "debugconfiguration.h"

#include <QJsonDocument>
#include <QFile>
#include <QDebug>

QJsonObject DebugConfiguration::toJson() const
{
    QJsonObject obj;
    obj["name"] = name;
    obj["type"] = type;
    obj["request"] = request;
    obj["program"] = program;
    obj["args"] = QJsonArray::fromStringList(args);
    obj["cwd"] = cwd;
    if (!debuggerPath.isEmpty()) {
        obj["debuggerPath"] = debuggerPath;
    }
    return obj;
}

DebugConfiguration DebugConfiguration::fromJson(const QJsonObject &obj)
{
    DebugConfiguration config;
    config.name = obj["name"].toString();
    config.type = obj["type"].toString();
    config.request = obj["request"].toString();
    config.program = obj["program"].toString();
    
    if (obj.contains("args")) {
        QJsonArray argsArray = obj["args"].toArray();
        for (const QJsonValue &val : argsArray) {
            config.args.append(val.toString());
        }
    }
    
    config.cwd = obj["cwd"].toString();
    config.debuggerPath = obj["debuggerPath"].toString();
    
    return config;
}

DebugConfigurationManager::DebugConfigurationManager()
{
}

void DebugConfigurationManager::loadFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open debug configuration file:" << path;
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse debug configuration:" << error.errorString();
        return;
    }
    
    m_configurations.clear();
    
    if (doc.isArray()) {
        QJsonArray array = doc.array();
        for (const QJsonValue &val : array) {
            if (val.isObject()) {
                m_configurations.append(DebugConfiguration::fromJson(val.toObject()));
            }
        }
    } else if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("configurations") && obj["configurations"].isArray()) {
            QJsonArray array = obj["configurations"].toArray();
            for (const QJsonValue &val : array) {
                if (val.isObject()) {
                    m_configurations.append(DebugConfiguration::fromJson(val.toObject()));
                }
            }
        }
    }
}

void DebugConfigurationManager::saveToFile(const QString &path)
{
    QJsonArray array;
    for (const DebugConfiguration &config : m_configurations) {
        array.append(config.toJson());
    }
    
    QJsonObject root;
    root["version"] = "0.2.0";
    root["configurations"] = array;
    
    QJsonDocument doc(root);
    
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open debug configuration file for writing:" << path;
        return;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

QList<DebugConfiguration> DebugConfigurationManager::configurations() const
{
    return m_configurations;
}

void DebugConfigurationManager::addConfiguration(const DebugConfiguration &config)
{
    m_configurations.append(config);
}

void DebugConfigurationManager::removeConfiguration(const QString &name)
{
    for (int i = 0; i < m_configurations.size(); ++i) {
        if (m_configurations[i].name == name) {
            m_configurations.removeAt(i);
            break;
        }
    }
}

DebugConfiguration DebugConfigurationManager::configuration(const QString &name) const
{
    for (const DebugConfiguration &config : m_configurations) {
        if (config.name == name) {
            return config;
        }
    }
    return DebugConfiguration();
}

bool DebugConfigurationManager::hasConfiguration(const QString &name) const
{
    for (const DebugConfiguration &config : m_configurations) {
        if (config.name == name) {
            return true;
        }
    }
    return false;
}
