#include "plugincontext.h"
#include "eventbus.h"
#include "servicelocator.h"
#include "pluginmanager.h"
#include "mainwindow.h"
#include <QDebug>
#include <QApplication>

PluginContext::PluginContext(MainWindow* mainWindow, QObject* parent)
    : QObject(parent)
    , m_mainWindow(mainWindow)
    , m_settings(nullptr)
{
    // 初始化 QSettings
    if (mainWindow) {
        m_settings = new QSettings(this);
    }
}

PluginContext::~PluginContext()
{
}

MainWindow* PluginContext::mainWindow() const
{
    return m_mainWindow;
}

QSettings* PluginContext::settings() const
{
    return m_settings;
}

CodeEditor* PluginContext::currentEditor() const
{
    if (!m_mainWindow) return nullptr;
    return m_mainWindow->getCurrentCodeEditor();
}

LspClient* PluginContext::lspClient() const
{
    if (!m_mainWindow) return nullptr;
    return m_mainWindow->getLspClient();
}

ProblemPanel* PluginContext::problemPanel() const
{
    if (!m_mainWindow) return nullptr;
    return m_mainWindow->getProblemPanel();
}

TerminalPanel* PluginContext::terminalPanel() const
{
    if (!m_mainWindow) return nullptr;
    return m_mainWindow->getTerminalPanel();
}

GitPanel* PluginContext::gitPanel() const
{
    if (!m_mainWindow) return nullptr;
    return m_mainWindow->getGitPanel();
}

QString PluginContext::currentProjectPath() const
{
    if (!m_mainWindow) return QString();
    return m_mainWindow->currentProjectPath();
}

QObject* PluginContext::getPlugin(const QString& id) const
{
    // 通過 PluginManager 獲取插件實例
    PluginManager* manager = qApp->findChild<PluginManager*>();
    if (manager) {
        ScripturaPlugin* plugin = manager->getPlugin(id);
        if (plugin) {
            // ScripturaPlugin 現在繼承 QObject，可以直接轉換
            return qobject_cast<QObject*>(plugin);
        }
    }
    return nullptr;
}

void PluginContext::notify(const QString& event, const QVariant& data)
{
    EventBus::instance()->publish(event, data);
}

EventBus::SubscriptionId PluginContext::subscribe(const QString& event, std::function<void(const QVariant&)> callback, QObject* owner)
{
    // 訂閱事件並儲存訂閱 ID (綁定擁有者生命週期，避免 use-after-free)
    EventBus::SubscriptionId id = EventBus::instance()->subscribe(event, owner, callback);
    m_eventHandlers[event].append({id, callback});
    return id;
}

void PluginContext::unsubscribe(const QString& event, EventBus::SubscriptionId subscriptionId)
{
    // 從 EventBus 取消訂閱
    EventBus::instance()->unsubscribe(event, subscriptionId);
    
    // 從本地記錄中移除
    auto& subscriptions = m_eventHandlers[event];
    for (auto it = subscriptions.begin(); it != subscriptions.end(); ++it) {
        if (it->id == subscriptionId) {
            subscriptions.erase(it);
            break;
        }
    }
    
    // 如果沒有訂閱者了，移除該事件
    if (subscriptions.isEmpty()) {
        m_eventHandlers.remove(event);
    }
}