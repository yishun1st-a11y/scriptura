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
            // 假設插件實例本身就是 QObject 子類
            return dynamic_cast<QObject*>(plugin);
        }
    }
    return nullptr;
}

void PluginContext::notify(const QString& event, const QVariant& data)
{
    EventBus::instance()->publish(event, data);
}

void PluginContext::subscribe(const QString& event, std::function<void(const QVariant&)> callback)
{
    // 訂閱事件並儲存訂閱 ID
    EventBus::SubscriptionId id = EventBus::instance()->subscribe(event, callback);
    m_eventHandlers[event].append({id, callback});
}