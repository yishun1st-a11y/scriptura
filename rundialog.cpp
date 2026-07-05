#include "rundialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

RunDialog::RunDialog(QWidget *parent)
    : QDialog(parent)
    , m_configCombo(new QComboBox(this))
    , m_runButton(new QPushButton(tr("Run"), this))
    , m_debugButton(new QPushButton(tr("Debug"), this))
    , m_editButton(new QPushButton(tr("Edit Configurations"), this))
    , m_cancelButton(new QPushButton(tr("Cancel"), this))
    , m_selectedMode("run")
{
    setWindowTitle(tr("Run / Debug"));
    setMinimumWidth(400);
    
    QLabel *label = new QLabel(tr("Select configuration:"), this);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addWidget(m_configCombo);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_runButton);
    buttonLayout->addWidget(m_debugButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    connect(m_runButton, &QPushButton::clicked, this, &RunDialog::onRunClicked);
    connect(m_debugButton, &QPushButton::clicked, this, &RunDialog::onDebugClicked);
    connect(m_editButton, &QPushButton::clicked, this, &RunDialog::onEditConfigurations);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void RunDialog::setConfigurations(const QList<DebugConfiguration> &configs)
{
    m_configCombo->clear();
    for (const DebugConfiguration &config : configs) {
        m_configCombo->addItem(config.name, config.name);
    }
    
    if (m_configCombo->count() > 0) {
        m_configCombo->setCurrentIndex(0);
    }
}

DebugConfiguration RunDialog::selectedConfiguration() const
{
    QString name = m_configCombo->currentData().toString();
    // The caller needs to look up the configuration by name
    DebugConfiguration config;
    config.name = name;
    return config;
}

QString RunDialog::selectedMode() const
{
    return m_selectedMode;
}

void RunDialog::onRunClicked()
{
    m_selectedMode = "run";
    accept();
}

void RunDialog::onDebugClicked()
{
    m_selectedMode = "debug";
    accept();
}

void RunDialog::onEditConfigurations()
{
    QMessageBox::information(this, tr("Edit Configurations"),
                             tr("Configuration editing will be implemented in a future update.\n"
                                "For now, edit the .vscode/launch.json file manually."));
}
