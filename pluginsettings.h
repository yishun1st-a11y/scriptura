#ifndef PLUGINSETTINGS_H
#define PLUGINSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QJsonObject>
#include <QVariant>

/**
 * @file pluginsettings.h
 * @brief 定義插件設定管理類別
 * 
 * PluginSettings 提供了插件專用的設定存取介面，
 * 允許插件儲存和讀取自己的設定值。
 */

/**
 * @class PluginSettings
 * @brief 管理插件設定的讀取和儲存
 * 
 * 每個插件都有獨立的設定空間，設定會自動以插件 ID 為前綴儲存。
 */
class PluginSettings : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 建構函數
     * @param pluginId 插件 ID
     * @param parent 父 QSettings (可選)
     */
    explicit PluginSettings(const QString& pluginId, QSettings* parent = nullptr);
    
    /**
     * @brief 解構函數
     */
    ~PluginSettings() override;

    /**
     * @brief 讀取設定值
     * @param key 設定鍵名
     * @param defaultValue 預設值
     * @return 設定值
     */
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    
    /**
     * @brief 儲存設定值
     * @param key 設定鍵名
     * @param value 設定值
     */
    void setValue(const QString& key, const QVariant& value);

    /**
     * @brief 類型安全的設定讀取
     * @tparam T 設定值類型
     * @param key 設定鍵名
     * @param defaultValue 預設值
     * @return 設定值
     */
    template<typename T>
    T value(const QString& key, const T& defaultValue = T()) const;

    /**
     * @brief 開始設定分組
     * @param prefix 分組前綴
     */
    void beginGroup(const QString& prefix);
    
    /**
     * @brief 結束設定分組
     */
    void endGroup();
    
    /**
     * @brief 設定預設值
     * @param defaults 預設值物件
     */
    void setDefaults(const QJsonObject& defaults);
    
    /**
     * @brief 重設為預設值
     */
    void resetToDefaults();
    
    /**
     * @brief 同步設定到磁碟
     */
    void sync();

private:
    QString m_pluginId;
    QSettings* m_settings;
    QJsonObject m_defaults;
    bool m_ownsSettings;
};

// 模板方法實作
template<typename T>
T PluginSettings::value(const QString& key, const T& defaultValue) const
{
    QVariant val = value(key, QVariant::fromValue(defaultValue));
    return val.value<T>();
}

#endif // PLUGINSETTINGS_H