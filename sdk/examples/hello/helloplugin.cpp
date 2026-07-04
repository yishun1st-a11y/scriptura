#include "helloplugin.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>

HelloPlugin::HelloPlugin(QObject* parent)
    : QObject(parent)
    , m_context(nullptr)
    , m_action(nullptr)
    , m_panel(nullptr)
    , m_label(nullptr)
{
}

HelloPlugin::~HelloPlugin()
{
    shutdown();
}

bool HelloPlugin::initialize(PluginContext* context)
{
    m_context = context;
    if (!m_context) {
        qWarning() << "HelloPlugin: Invalid plugin context";
        return false;
    }

    // 創建選單動作
    if (m_context->mainWindow()) {
        m_action = new QAction(tr("Say Hello"), m_context->mainWindow());
        connect(m_action, &QAction::triggered, this, &HelloPlugin::sayHello);
        m_context->mainWindow()->menuBar()->addAction(m_action);
    }

    // 創建設置面板
    m_panel = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_panel);
    m_label = new QLabel(tr("Hello from HelloPlugin!"), m_panel);
    layout->addWidget(m_label);
    layout->addStretch();

    qDebug() << "HelloPlugin initialized successfully";
    return true;
}

void HelloPlugin::shutdown()
{
    if (m_action) {
        delete m_action;
        m_action = nullptr;
    }
    if (m_panel) {
        delete m_panel;
        m_panel = nullptr;
        m_label = nullptr;
    }
    m_context = nullptr;
    qDebug() << "HelloPlugin shutdown";
}

bool HelloPlugin::hasFeature(PluginFeature feature) const
{
    switch (feature) {
        case PluginFeature::MenuAction:
            return true;
        default:
            return false;
    }
}

void HelloPlugin::sayHello()
{
    QMessageBox::information(nullptr, tr("Hello"), 
        tr("Hello from HelloPlugin! This is a demonstration of the Scriptura plugin system."));
}
