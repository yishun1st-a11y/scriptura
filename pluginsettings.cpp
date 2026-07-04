#include "pluginsettings.h"
#include <QDebug>
#include <QStandardPaths>

PluginSettings::PluginSettings(const QString& pluginId, QSettings* parent)
    : QObject(nullptr)
    , m_pluginId(pluginId)
    , m_settings(parent)
    , m_ownsSettings(false)
{
    if (!m_settings) {
        QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/plugins/" + m_pluginId + ".ini";
        m_settings = new QSettings(settingsPath, QSettings::IniFormat);
        m_ownsSettings = true;
    }
}

PluginSettings::~PluginSettings()
{
    if (m_ownsSettings && m_settings) {
        delete m_settings;
    }
}

QVariant PluginSettings::value(const QString& key, const QVariant& defaultValue) const
{
    // 優先使用預設值
    if (m_defaults.contains(key)) {
        QVariant defaultVal = m_defaults[key].toVariant();
        m_settings->beginGroup(m_pluginId);
        QVariant result = m_settings->value(key, defaultVal);
        m_settings->endGroup();
        return result;
    }
    
    m_settings->beginGroup(m_pluginId);
    QVariant result = m_settings->value(key, defaultValue);
    m_settings->endGroup();
    return result;
}

void PluginSettings::setValue(const QString& key, const QVariant& value)
{
    m_settings->beginGroup(m_pluginId);
    m_settings->setValue(key, value);
    m_settings->endGroup();
}

void PluginSettings::beginGroup(const QString& prefix)
{
    m_settings->beginGroup(m_pluginId + "/" + prefix);
}

void PluginSettings::endGroup()
{
    m_settings->endGroup();
}

void PluginSettings::setDefaults(const QJsonObject& defaults)
{
    m_defaults = defaults;
}

void PluginSettings::resetToDefaults()
{
    m_settings->remove(m_pluginId);
}

void PluginSettings::sync()
{
    m_settings->sync();
}