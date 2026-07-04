#ifndef GITPLUGIN_H
#define GITPLUGIN_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QProcess>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include "plugininterface.h"
#include "plugincontext.h"

/**
 * @file gitplugin.h
 * @brief Git 版本控制插件
 * 
 * 這個插件提供 Git 版本控制整合功能，包括：
 * - Git 命令執行
 * - 提交和推送
 * - 狀態顯示
 * - 分支管理
 */

/**
 * @class GitPlugin
 * @brief Git 版本控制插件實現
 * 
 * 此插件將 GitPanel 重構為插件形式，提供：
 * - Git 命令執行介面
 * - 輸出顯示
 * - 常用操作按鈕
 */
class GitPlugin : public QObject, public ScripturaPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.scriptura.plugin/1.0" FILE "git.json")
    Q_INTERFACES(ScripturaPlugin)

public:
    /**
     * @brief 建構函數
     */
    explicit GitPlugin(QObject* parent = nullptr);
    
    /**
     * @brief 解構函數
     */
    ~GitPlugin() override;

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
     * @return "com.scriptura.git"
     */
    QString id() const override { return "com.scriptura.git"; }
    
    /**
     * @brief 獲取插件名稱
     * @return "Git Integration"
     */
    QString name() const override { return "Git Integration"; }
    
    /**
     * @brief 獲取插件版本
     * @return "1.0.0"
     */
    QString version() const override { return "1.0.0"; }
    
    /**
     * @brief 獲取插件作者
     * @return "Scriptura Team"
     */
    QString author() const override { return "Scriptura Team"; }
    
    /**
     * @brief 獲取插件描述
     * @return 插件功能描述
     */
    QString description() const override { 
        return "Git version control integration with commit, push, and status viewing"; 
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

    /**
     * @brief 獲取插件面板 Widget
     * @return Git 面板 Widget
     */
    QWidget* panel() const { return m_panel; }

public slots:
    /**
     * @brief 設置輸出文字
     * @param text 要顯示的文字
     */
    void setOutput(const QString& text);

signals:
    /**
     * @brief 提交請求信號
     */
    void commitRequested();
    
    /**
     * @brief 推送請求信號
     */
    void pushRequested();

private slots:
    /**
     * @brief 提交按鈕點擊處理
     */
    void onCommitClicked();
    
    /**
     * @brief 推送按鈕點擊處理
     */
    void onPushClicked();

private:
    PluginContext* m_context;          ///< 插件上下文
    QWidget* m_panel;                  ///< 插件面板
    QTextEdit* m_outputEdit;           ///< 輸出顯示區域
    QVBoxLayout* m_mainLayout;         ///< 主佈局
    QPushButton* m_commitButton;       ///< 提交按鈕
    QPushButton* m_pushButton;         ///< 推送按鈕
    QProcess* m_gitProcess;            ///< Git 進程
    QString m_projectPath;             ///< 專案路徑
};

#endif // GITPLUGIN_H