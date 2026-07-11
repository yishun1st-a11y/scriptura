#ifndef PLUGINCONTEXT_H
#define PLUGINCONTEXT_H

#include <QObject>
#include <QVariant>
#include <QSettings>
#include <functional>
#include "plugininterface.h"
#include "eventbus.h"

/**
 * @file plugincontext.h
 * @brief 定義插件上下文，提供對核心服務的訪問
 * 
 * PluginContext 是插件與主應用程式之間的橋樑，
 * 它提供了對各種服務和功能的訪問介面。
 */

class MainWindow;
class CodeEditor;
class LspClient;
class ProblemPanel;
class TerminalPanel;
class GitPanel;

/**
 * @class PluginContext
 * @brief 插件上下文，提供對核心服務的訪問
 * 
 * 所有服務都以抽象介面的形式提供，允許插件：
 * - 存取主視窗和 UI 元件
 * - 使用設定系統
 * - 與其他插件通訊
 * - 訂閱和發佈事件
 */
class PluginContext : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 訂閱資訊
     */
    struct Subscription {
        quint64 id;
        std::function<void(const QVariant&)> callback;
    };

    /**
     * @brief 建構函數
     * @param mainWindow 主視窗指標
     * @param parent 父物件
     */
    explicit PluginContext(MainWindow* mainWindow, QObject* parent = nullptr);
    
    /**
     * @brief 解構函數
     */
    ~PluginContext() override;

    // 核心服務
    
    /**
     * @brief 獲取主視窗
     * @return 主視窗指標
     */
    MainWindow* mainWindow() const;
    
    /**
     * @brief 獲取設定物件
     * @return QSettings 指標
     */
    QSettings* settings() const;
    
    // 編輯器服務
    
    /**
     * @brief 獲取當前編輯器
     * @return 當前 CodeEditor 指標，如果沒有開啟的檔案則返回 nullptr
     */
    CodeEditor* currentEditor() const;
    
    /**
     * @brief 獲取 LSP 客戶端
     * @return LspClient 指標
     */
    LspClient* lspClient() const;
    
    // 面板服務
    
    /**
     * @brief 獲取問題面板
     * @return ProblemPanel 指標
     */
    ProblemPanel* problemPanel() const;
    
    /**
     * @brief 獲取終端機面板
     * @return TerminalPanel 指標
     */
    TerminalPanel* terminalPanel() const;
    
    /**
      * @brief 獲取 Git 面板
      * @return GitPanel 指標
      */
     GitPanel* gitPanel() const;
     
     /**
      * @brief 獲取當前專案路徑
      * @return 專案路徑字串，如果未開啟專案則返回空
      */
     QString currentProjectPath() const;

     // 插件間通訊
    
    /**
     * @brief 獲取指定 ID 的插件實例
     * @param id 插件 ID
     * @return 插件物件指標，如果未找到則返回 nullptr
     */
    QObject* getPlugin(const QString& id) const;
    
    /**
     * @brief 獲取指定 ID 的插件實例（類型安全版本）
     * @tparam T 插件類型
     * @param id 插件 ID
     * @return 插件物件指標，如果未找到則返回 nullptr
     */
    template<typename T>
    T getPlugin(const QString& id) const;

    // 事件系統
    
    /**
     * @brief 發佈事件
     * @param event 事件名稱
     * @param data 事件資料 (可選)
     */
    void notify(const QString& event, const QVariant& data = QVariant());
    
    /**
     * @brief 訂閱事件
     * @param event 事件名稱
     * @param callback 事件回調函數
     * @return 訂閱 ID，可用於取消訂閱
     */
    EventBus::SubscriptionId subscribe(const QString& event, std::function<void(const QVariant&)> callback);
    
    /**
     * @brief 取消訂閱事件
     * @param event 事件名稱
     * @param subscriptionId 訂閱 ID
     */
    void unsubscribe(const QString& event, EventBus::SubscriptionId subscriptionId);

private:
    MainWindow* m_mainWindow;
    QSettings* m_settings;
    
    // 事件處理器存儲
    QHash<QString, QList<Subscription>> m_eventHandlers;
};

// 模板方法實作
template<typename T>
T PluginContext::getPlugin(const QString& id) const
{
    QObject* plugin = getPlugin(id);
    return plugin ? qobject_cast<T>(plugin) : nullptr;
}

#endif // PLUGINCONTEXT_H