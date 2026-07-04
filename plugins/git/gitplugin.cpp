#include "gitplugin.h"
#include <QApplication>
#include <QProcess>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>

GitPlugin::GitPlugin(QObject* parent)
    : QObject(parent)
    , m_context(nullptr)
    , m_panel(nullptr)
    , m_outputEdit(nullptr)
    , m_mainLayout(nullptr)
    , m_commitButton(nullptr)
    , m_pushButton(nullptr)
    , m_gitProcess(nullptr)
    , m_projectPath(QString())
{
}

GitPlugin::~GitPlugin()
{
    shutdown();
}

bool GitPlugin::initialize(PluginContext* context)
{
    m_context = context;
    
    if (!m_context) {
        qWarning() << "GitPlugin: Invalid plugin context";
        return false;
    }
    
    // 創建面板
    m_panel = new QWidget();
    m_panel->setObjectName("gitPanel");
    
    // 創建 UI 元件
    m_outputEdit = new QTextEdit(m_panel);
    m_outputEdit->setReadOnly(true);
    m_outputEdit->setPlaceholderText(tr("Git output will appear here..."));
    m_outputEdit->setStyleSheet(
        "QTextEdit { border: none; background: palette(base); font-family: monospace; }"
    );
    
    // Header with action buttons
    QWidget* headerWidget = new QWidget(m_panel);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(4, 4, 4, 4);
    headerLayout->setSpacing(4);
    
    m_commitButton = new QPushButton(tr("Commit"), headerWidget);
    m_pushButton = new QPushButton(tr("Push"), headerWidget);
    
    headerLayout->addWidget(m_commitButton);
    headerLayout->addWidget(m_pushButton);
    headerLayout->addStretch();
    
    // Main layout
    m_mainLayout = new QVBoxLayout(m_panel);
    m_mainLayout->addWidget(headerWidget);
    m_mainLayout->addWidget(m_outputEdit);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);
    m_mainLayout->setSpacing(4);
    
    // 連接信號槽
    connect(m_commitButton, &QPushButton::clicked, this, &GitPlugin::onCommitClicked);
    connect(m_pushButton, &QPushButton::clicked, this, &GitPlugin::onPushClicked);
    
    // 獲取當前專案路徑
    if (m_context->mainWindow()) {
        m_projectPath = m_context->currentProjectPath();
    }
    
    qDebug() << "GitPlugin initialized successfully";
    return true;
}

void GitPlugin::shutdown()
{
    if (m_gitProcess) {
        if (m_gitProcess->state() != QProcess::NotRunning) {
            m_gitProcess->kill();
            m_gitProcess->waitForFinished();
        }
        delete m_gitProcess;
        m_gitProcess = nullptr;
    }
    
    if (m_panel) {
        delete m_panel;
        m_panel = nullptr;
        m_outputEdit = nullptr;
        m_mainLayout = nullptr;
        m_commitButton = nullptr;
        m_pushButton = nullptr;
    }
    
    m_context = nullptr;
    qDebug() << "GitPlugin shutdown";
}

bool GitPlugin::hasFeature(PluginFeature feature) const
{
    switch (feature) {
        case PluginFeature::ToolPanel:
        case PluginFeature::VCSIntegration:
            return true;
        default:
            return false;
    }
}

void GitPlugin::setOutput(const QString& text)
{
    if (m_outputEdit) {
        m_outputEdit->setPlainText(text);
    }
}

void GitPlugin::onCommitClicked()
{
    if (!m_context || !m_outputEdit) {
        return;
    }
    
    // 獲取 Git 狀態
    QProcess gitStatus;
    gitStatus.setWorkingDirectory(m_projectPath);
    gitStatus.start("git", QStringList() << "status" << "--short");
    
    if (!gitStatus.waitForFinished(3000)) {
        QMessageBox::warning(m_panel, tr("Git Error"), 
                           tr("Failed to get git status: %1").arg(gitStatus.errorString()));
        return;
    }
    
    QString status = gitStatus.readAllStandardOutput();
    if (status.isEmpty()) {
        QMessageBox::information(m_panel, tr("Git"), tr("No changes to commit"));
        return;
    }
    
    // 顯示狀態並請求提交訊息
    QString commitMessage = QInputDialog::getText(m_panel, 
                                                  tr("Commit"), 
                                                  tr("Enter commit message:"),
                                                  QLineEdit::Normal,
                                                  QString());
    
    if (commitMessage.isEmpty()) {
        return;
    }
    
    // 執行提交
    m_gitProcess = new QProcess(this);
    connect(m_gitProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0) {
            setOutput(tr("Commit successful!\n\n") + m_gitProcess->readAllStandardOutput());
            emit commitRequested();
        } else {
            setOutput(tr("Commit failed:\n%1").arg(m_gitProcess->readAllStandardError()));
        }
        m_gitProcess->deleteLater();
        m_gitProcess = nullptr;
    });
    
    m_gitProcess->setWorkingDirectory(m_projectPath);
    m_gitProcess->start("git", QStringList() << "commit" << "-m" << commitMessage);
    
    m_outputEdit->setText(tr("Committing...\n"));
}

void GitPlugin::onPushClicked()
{
    if (!m_context || !m_outputEdit) {
        return;
    }
    
    // 執行推送
    m_gitProcess = new QProcess(this);
    connect(m_gitProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0) {
            setOutput(tr("Push successful!\n\n") + m_gitProcess->readAllStandardOutput());
            emit pushRequested();
        } else {
            setOutput(tr("Push failed:\n%1").arg(m_gitProcess->readAllStandardError()));
        }
        m_gitProcess->deleteLater();
        m_gitProcess = nullptr;
    });
    
    m_gitProcess->setWorkingDirectory(m_projectPath);
    m_gitProcess->start("git", QStringList() << "push");
    
    m_outputEdit->setText(tr("Pushing...\n"));
}