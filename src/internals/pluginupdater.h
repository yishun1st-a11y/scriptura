#ifndef PLUGINUPDATER_H
#define PLUGINUPDATER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QString>
#include <QList>

/**
 * @file pluginupdater.h
 * @brief 插件更新管理器，負責檢查和下載插件更新
 */

/**
 * @struct UpdateInfo
 * @brief 插件更新資訊結構
 */
struct UpdateInfo {
    QString pluginId;           ///< 插件 ID
    QString currentVersion;     ///< 當前版本
    QString latestVersion;      ///< 最新版本
    QString downloadUrl;        ///< 下載連結
    QString changelog;          ///< 更新日誌
    bool securityUpdate;        ///< 是否為安全更新
};

/**
 * @class PluginUpdater
 * @brief 管理插件更新的檢查、下載和安裝
 */
class PluginUpdater : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 建構函數
     * @param parent 父物件
     */
    explicit PluginUpdater(QObject* parent = nullptr);
    
    /**
     * @brief 解構函數
     */
    ~PluginUpdater();

    /**
     * @brief 檢查所有已安裝插件的更新
     * @return 可用更新列表
     */
    QList<UpdateInfo> checkForUpdates();

    /**
     * @brief 下載插件更新
     * @param update 更新資訊
     * @return 成功返回 true
     */
    bool downloadUpdate(const UpdateInfo& update);

    /**
     * @brief 安裝插件更新
     * @param pluginId 插件 ID
     * @return 成功返回 true
     */
    bool installUpdate(const QString& pluginId);

    /**
     * @brief 設定自動更新檢查
     * @param intervalHours 檢查間隔（小時）
     */
    void scheduleUpdateCheck(int intervalHours = 24);

    /**
     * @brief 設定註冊表 URL
     * @param url 註冊表 URL
     */
    void setRegistryUrl(const QString& url);

signals:
    /**
     * @brief 發現更新信號
     * @param updates 可用更新列表
     */
    void updatesAvailable(const QList<UpdateInfo>& updates);

    /**
     * @brief 沒有可用更新信號
     */
    void noUpdatesAvailable();

    /**
     * @brief 更新檢查失敗信號
     * @param error 錯誤訊息
     */
    void updateCheckFailed(const QString& error);

    /**
     * @brief 插件更新完成信號
     * @param pluginId 插件 ID
     */
    void pluginUpdated(const QString& pluginId);

private slots:
    /**
     * @brief 網路回覆處理
     * @param reply 網路回覆
     */
    void onRegistryReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_networkManager;  ///< 網路存取管理器
    QTimer* m_updateTimer;                    ///< 更新檢查定時器
    QString m_registryUrl;                    ///< 註冊表 URL
};

#endif // PLUGINUPDATER_H
