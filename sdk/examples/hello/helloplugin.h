#ifndef HELLOPLUGIN_H
#define HELLOPLUGIN_H

#include <QObject>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QAction>
#include "../include/scriptura/plugininterface.h"
#include "../include/scriptura/plugincontext.h"

/**
 * @file helloplugin.h
 * @brief 簡單的 Hello World 插件範例
 */

/**
 * @class HelloPlugin
 * @brief 最簡單的 Scriptura 插件實現
 */
class HelloPlugin : public QObject, public ScripturaPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.scriptura.plugin/1.0" FILE "plugin.json")
    Q_INTERFACES(ScripturaPlugin)

public:
    /**
     * @brief 建構函數
     */
    explicit HelloPlugin(QObject* parent = nullptr);
    
    /**
     * @brief 解構函數
     */
    ~HelloPlugin() override;

    // ScripturaPlugin 介面實作

    /**
     * @brief 初始化插件
     * @param context 插件上下文
     * @return 初始化成功返回 true
     */
    bool initialize(PluginContext* context) override;
    
    /**
     * @brief 關閉插件
     */
    void shutdown() override;
    
    /**
     * @brief 獲取插件 ID
     * @return "com.scriptura.hello"
     */
    QString id() const override { return "com.scriptura.hello"; }
    
    /**
     * @brief 獲取插件名稱
     * @return "Hello World"
     */
    QString name() const override { return "Hello World"; }
    
    /**
     * @brief 獲取插件版本
     * @return "1.0.0"
     */
    QString version() const override { return "1.0.0"; }
    
    /**
     * @brief 獲取插件作者
     * @return "Scriptura"
     */
    QString author() const override { return "Scriptura"; }
    
    /**
     * @brief 獲取插件描述
     * @return 插件功能描述
     */
    QString description() const override { 
        return "A simple hello world plugin that demonstrates the plugin API"; 
    }
    
    /**
     * @brief 獲取依賴列表
     * @return 空列表（無依賴）
     */
    QStringList dependencies() const override { return {}; }
    
    /**
     * @brief 檢查是否支援特定功能
     * @param feature 功能特徵
     * @return 是否支援
     */
    bool hasFeature(PluginFeature feature) const override;

public slots:
    /**
     * @brief 顯示問候訊息
     */
    void sayHello();

private:
    PluginContext* m_context;          ///< 插件上下文
    QAction* m_action;                 ///< 選單動作
    QWidget* m_panel;                  ///< 插件面板
    QLabel* m_label;                   ///< 標籤
};

#endif // HELLOPLUGIN_H
