#ifndef PLUGINCRASHHANDLER_H
#define PLUGINCRASHHANDLER_H

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QList>

/**
 * @file plugincrashhandler.h
 * @brief 插件崩潰處理器，負責監控和處理插件崩潰
 */

/**
 * @struct CrashInfo
 * @brief 崩潰資訊結構
 */
struct CrashInfo {
    QString pluginId;          ///< 插件 ID
    QDateTime timestamp;       ///< 崩潰時間
    QString errorType;         ///< 錯誤類型
    QString stackTrace;        ///< 堆疊追蹤
    bool autoDisabled;         ///< 是否自動禁用
};

/**
 * @class PluginCrashHandler
 * @brief 處理插件崩潰並執行恢復策略
 */
class PluginCrashHandler : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 建構函數
     * @param parent 父物件
     */
    explicit PluginCrashHandler(QObject* parent = nullptr);
    
    /**
     * @brief 解構函數
     */
    ~PluginCrashHandler();

    /**
     * @brief 註冊插件進程
     * @param pluginId 插件 ID
     * @param process 進程指標
     */
    void registerPluginProcess(const QString& pluginId, class QProcess* process);

    /**
     * @brief 處理插件崩潰
     * @param pluginId 插件 ID
     */
    void handleCrash(const QString& pluginId);

    /**
     * @brief 禁用指定插件
     * @param pluginId 插件 ID
     */
    void disablePlugin(const QString& pluginId);

    /**
     * @brief 獲取最近崩潰記錄
     * @param limit 最大記錄數
     * @return 崩潰資訊列表
     */
    QList<CrashInfo> recentCrashes(int limit = 10);

    /**
     * @brief 檢查插件是否被禁用
     * @param pluginId 插件 ID
     * @return 被禁用返回 true
     */
    bool isPluginDisabled(const QString& pluginId) const;

    /**
     * @brief 恢復禁用的插件
     * @param pluginId 插件 ID
     */
    void enablePlugin(const QString& pluginId);

signals:
    /**
     * @brief 插件崩潰信號
     * @param pluginId 插件 ID
     * @param info 崩潰資訊
     */
    void pluginCrashed(const QString& pluginId, const CrashInfo& info);

private:
    QList<CrashInfo> m_crashHistory;                           ///< 崩潰歷史記錄
    QHash<QString, QProcess*> m_pluginProcesses;               ///< 插件進程映射
    QHash<QString, bool> m_disabledPlugins;                    ///< 禁用插件列表
    QString m_crashLogPath;                                    ///< 崩潰日誌路徑
};

#endif // PLUGINCRASHHANDLER_H
