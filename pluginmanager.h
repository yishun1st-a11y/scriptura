#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QPluginLoader>
#include <QJsonObject>
#include <QHash>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVariant>
#include <functional>
#include "plugininterface.h"
#include "dependencyresolver.h"

/**
 * @file pluginmanager.h
 * @brief 定義插件管理器，負責插件的載入、卸載和生命週期管理
 * 
 * PluginManager 是插件系統的核心管理類別，負責：
 * - 插件發現和掃描
 * - 依賴解析和載入順序計算
 * - 插件生命週期管理
 * - 事件系統協調
 */

/**
 * @class PluginManager
 * @brief 管理所有插件的載入、卸載和生命週期
 * 
 * 這個類別實作了插件系統的核心邏輯，包括：
 * - 掃描插件目錄發現可用插件
 * - 解析插件元數據
 * - 處理插件依賴關係
 * - 管理插件載入順序
 * - 處理插件事件
 */
class PluginManager : public QObject
{
    Q_OBJECT

public:
     /**
      * @brief 建構函數
      * @param parent 父物件
      */
     explicit PluginManager(QObject* parent = nullptr);
     
     /**
      * @brief 設定插件上下文
      * @param context 插件上下文指標
      */
     void setContext(PluginContext* context);
     
     /**
      * @brief 解構函數
      */
     ~PluginManager() override;

    // 插件生命週期
    
    /**
     * @brief 從指定目錄載入所有插件
     * @param pluginPath 插件目錄路徑
     * @return 成功載入返回 true
     */
    bool loadPlugins(const QString& pluginPath);
    
    /**
     * @brief 載入單一插件
     * @param filePath 插件庫檔案路徑
     * @return 成功載入返回 true
     */
    bool loadPlugin(const QString& filePath);
    
    /**
     * @brief 卸載指定插件
     * @param id 插件 ID
     */
    void unloadPlugin(const QString& id);
    
    /**
     * @brief 卸載所有插件
     */
    void unloadAllPlugins();

     // 插件查詢
     
     /**
      * @brief 獲取所有已載入的插件
      * @return 插件列表
      */
     QList<ScripturaPlugin*> plugins() const;
     
     /**
      * @brief 獲取指定 ID 的插件
      * @param id 插件 ID
      * @return 插件指標，如果未找到則返回 nullptr
      */
     ScripturaPlugin* getPlugin(const QString& id) const;
     
     /**
      * @brief 檢查插件是否已載入
      * @param id 插件 ID
      * @return 已載入返回 true
      */
     bool isLoaded(const QString& id) const;

     // 允許清單（僅 GUI 安裝）
     
     /**
      * @brief 設定允許載入的插件 ID 清單
      * @param allowed 允許的插件 ID 集合
      */
     void setAllowedPlugins(const QSet<QString>& allowed);
     
     /**
      * @brief 新增允許載入的插件 ID
      * @param id 插件 ID
      */
     void addAllowedPlugin(const QString& id);
     
     /**
      * @brief 移除允許載入的插件 ID
      * @param id 插件 ID
      */
     void removeAllowedPlugin(const QString& id);
     
     /**
      * @brief 檢查插件是否在允許清單中
      * @param id 插件 ID
      * @return 在允許清單中返回 true
      */
     bool isAllowed(const QString& id) const;
     
     /**
      * @brief 獲取允許載入的插件 ID 清單
      * @return 允許的插件 ID 集合
      */
     QSet<QString> allowedPlugins() const;

     // 功能查詢
    
    /**
     * @brief 獲取支援特定功能的插件列表
     * @param feature 功能特徵
     * @return 插件列表
     */
    QList<ScripturaPlugin*> pluginsWithFeature(PluginFeature feature) const;

    // 事件系統
    
    /**
     * @brief 發佈事件到所有訂閱者
     * @param event 事件名稱
     * @param data 事件資料
     */
    void publishEvent(const QString& event, const QVariant& data = QVariant());
    
    /**
     * @brief 訂閱事件
     * @param event 事件名稱
     * @param callback 回調函數
     * @return 訂閱 ID
     */
    quint64 subscribeToEvent(const QString& event, 
                             std::function<void(const QVariant&)> callback);

signals:
    /**
     * @brief 插件載入成功信號
     * @param id 插件 ID
     */
    void pluginLoaded(const QString& id);
    
    /**
     * @brief 插件卸載信號
     * @param id 插件 ID
     */
    void pluginUnloaded(const QString& id);
    
    /**
     * @brief 插件錯誤信號
     * @param id 插件 ID
     * @param error 錯誤訊息
     */
    void pluginError(const QString& id, const QString& error);

private:
    /**
     * @brief 載入插件元數據
     * @param filePath 插件目錄路徑
     * @param metadata 輸出的元數據物件
     * @return 成功載入返回 true
     */
    bool loadPluginMetadata(const QString& filePath, QJsonObject& metadata);
    
    /**
     * @brief 檢查插件依賴
     * @param metadata 插件元數據
     * @return 依賴滿足返回 true
     */
    bool checkDependencies(const QJsonObject& metadata);
    
    /**
     * @brief 解析所有插件依賴
     * @return 成功解析返回 true
     */
    bool resolveDependencies();
    
    /**
     * @brief 初始化所有已載入的插件
     */
    void initializePlugins();
    
    /**
     * @brief 設定插件信號連接
     * @param plugin 插件指標
     */
    void setupPluginConnections(ScripturaPlugin* plugin);
    
    /**
     * @brief 建立依賴圖
     * @param pluginMetadata 插件元數據列表
     * @return 成功建立返回 true
     */
    bool buildDependencyGraph(const QList<QJsonObject>& pluginMetadata);
    
    /**
     * @brief 執行拓撲排序
     * @return 排序後的插件 ID 列表
     */
    QStringList topologicalSort();
    
    /**
     * @brief 根據 ID 載入插件
     * @param pluginId 插件 ID
     * @return 成功載入返回 true
     */
    bool loadPluginById(const QString& pluginId);

    /**
     * @brief 插件資訊結構
     */
    struct PluginInfo {
        QString filePath;           ///< 插件目錄路徑
        QJsonObject metadata;       ///< 插件元數據
        QPluginLoader* loader;        ///< Qt 插件載入器
        ScripturaPlugin* instance;    ///< 插件實例
        bool initialized;           ///< 是否已初始化
        QStringList dependencies;   ///< 依賴列表
        
        PluginInfo() : loader(nullptr), instance(nullptr), initialized(false) {}
    };

    /**
     * @brief 訂閱資訊
     */
    struct Subscription {
        quint64 id;
        std::function<void(const QVariant&)> callback;
    };

    QHash<QString, PluginInfo> m_plugins;                           ///< 插件註冊表
    QHash<QString, QStringList> m_dependencyGraph;                  ///< 依賴圖
    QHash<QString, QString> m_pluginPaths;                          ///< 插件ID -> 目錄路徑
    QHash<QString, QList<Subscription>> m_eventHandlers;            ///< 事件處理器
    QSet<QString> m_allowedPlugins;                                 ///< 允許載入的插件 ID
    DependencyResolver m_resolver;                                  ///< 依賴解析器
    quint64 m_nextSubscriptionId = 1;                               ///< 下一個訂閱 ID
    PluginContext* m_context = nullptr;                             ///< 插件上下文
};

#endif // PLUGINMANAGER_H