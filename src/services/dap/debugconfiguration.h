#ifndef DEBUGCONFIGURATION_H
#define DEBUGCONFIGURATION_H

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

struct DebugConfiguration
{
    QString name;
    QString type;
    QString request;
    QString program;
    QStringList args;
    QString cwd;
    QString debuggerPath;
    QString processId;
    
    QJsonObject toJson() const;
    static DebugConfiguration fromJson(const QJsonObject &obj);
};

class DebugConfigurationManager
{
public:
    DebugConfigurationManager();
    
    void loadFromFile(const QString &path);
    void saveToFile(const QString &path);
    
    QList<DebugConfiguration> configurations() const;
    void setConfigurations(const QList<DebugConfiguration> &configs);
    void addConfiguration(const DebugConfiguration &config);
    void removeConfiguration(const QString &name);
    DebugConfiguration configuration(const QString &name) const;
    
    bool hasConfiguration(const QString &name) const;
    
private:
    QList<DebugConfiguration> m_configurations;
};

#endif // DEBUGCONFIGURATION_H
