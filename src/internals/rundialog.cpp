#include "rundialog.h"

#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>

RunDialog::RunDialog(QWidget *parent)
    : QDialog(parent)
    , m_configCombo(new QComboBox(this))
    , m_runButton(new QPushButton(tr("Run"), this))
    , m_debugButton(new QPushButton(tr("Debug"), this))
    , m_addButton(new QPushButton(tr("Add"), this))
    , m_deleteButton(new QPushButton(tr("Delete"), this))
    , m_editButton(new QPushButton(tr("Edit Configurations"), this))
    , m_applyButton(new QPushButton(tr("Apply"), this))
    , m_cancelButton(new QPushButton(tr("Cancel"), this))
    , m_selectedMode("run")
{
    setWindowTitle(tr("Run / Debug"));
    setMinimumWidth(450);

    QLabel *label = new QLabel(tr("Select configuration:"), this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addWidget(m_configCombo);

    // Configuration editor form
    QWidget *formWidget = new QWidget(this);
    QFormLayout *formLayout = new QFormLayout(formWidget);
    m_nameEdit = new QLineEdit(this);
    m_programEdit = new QLineEdit(this);
    m_argsEdit = new QLineEdit(this);
    m_cwdEdit = new QLineEdit(this);
    m_debuggerEdit = new QLineEdit(this);
    formLayout->addRow(tr("Name:"), m_nameEdit);
    formLayout->addRow(tr("Program:"), m_programEdit);
    formLayout->addRow(tr("Arguments:"), m_argsEdit);
    formLayout->addRow(tr("Working dir:"), m_cwdEdit);
    formLayout->addRow(tr("Debugger path:"), m_debuggerEdit);
    formWidget->setVisible(false);
    mainLayout->addWidget(formWidget);
    m_applyButton->setVisible(false);

    QHBoxLayout *editLayout = new QHBoxLayout();
    editLayout->addWidget(m_addButton);
    editLayout->addWidget(m_deleteButton);
    editLayout->addWidget(m_editButton);
    editLayout->addWidget(m_applyButton);
    mainLayout->addLayout(editLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_runButton);
    buttonLayout->addWidget(m_debugButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(m_runButton, &QPushButton::clicked, this, &RunDialog::onRunClicked);
    connect(m_debugButton, &QPushButton::clicked, this, &RunDialog::onDebugClicked);
    connect(m_addButton, &QPushButton::clicked, this, &RunDialog::onAddClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &RunDialog::onDeleteClicked);
    connect(m_editButton, &QPushButton::clicked, this, &RunDialog::onEditClicked);
    connect(m_applyButton, &QPushButton::clicked, this, &RunDialog::onApplyClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_configCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RunDialog::onSelectionChanged);
}

void RunDialog::setConfigurations(const QList<DebugConfiguration> &configs)
{
    m_configs = configs;
    m_configCombo->clear();
    for (const DebugConfiguration &config : m_configs)
        m_configCombo->addItem(config.name, config.name);
    if (m_configCombo->count() > 0)
        m_configCombo->setCurrentIndex(0);
}

void RunDialog::onSelectionChanged(int index)
{
    if (index >= 0 && index < m_configs.size())
        loadConfigIntoForm(m_configs.at(index));
}

void RunDialog::loadConfigIntoForm(const DebugConfiguration &config)
{
    m_nameEdit->setText(config.name);
    m_programEdit->setText(config.program);
    m_argsEdit->setText(config.args.join(" "));
    m_cwdEdit->setText(config.cwd);
    m_debuggerEdit->setText(config.debuggerPath);
}

DebugConfiguration RunDialog::selectedConfiguration() const
{
    int idx = m_configCombo->currentIndex();
    if (idx >= 0 && idx < m_configs.size())
        return m_configs.at(idx);
    DebugConfiguration config;
    config.name = m_configCombo->currentData().toString();
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

void RunDialog::onAddClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("New Configuration"),
                                         tr("Configuration name:"), QLineEdit::Normal, tr("New Config"), &ok);
    if (!ok || name.isEmpty())
        return;
    DebugConfiguration config;
    config.name = name;
    config.type = "cppdbg";
    config.request = "launch";
    config.program = "";
    config.args = QStringList();
    config.cwd = "";
    m_configs.append(config);
    m_configCombo->addItem(config.name, config.name);
    m_configCombo->setCurrentIndex(m_configCombo->count() - 1);
}

void RunDialog::onDeleteClicked()
{
    int idx = m_configCombo->currentIndex();
    if (idx < 0)
        return;
    m_configs.removeAt(idx);
    m_configCombo->removeItem(idx);
}

void RunDialog::onEditClicked()
{
    QWidget *form = m_nameEdit->parentWidget();
    bool visible = !form->isVisible();
    form->setVisible(visible);
    m_applyButton->setVisible(visible);
    if (visible)
        onSelectionChanged(m_configCombo->currentIndex());
}

void RunDialog::onApplyClicked()
{
    int idx = m_configCombo->currentIndex();
    if (idx < 0)
        return;
    DebugConfiguration config = m_configs.at(idx);
    config.name = m_nameEdit->text().trimmed();
    config.program = m_programEdit->text();
    config.args = m_argsEdit->text().split(" ", Qt::SkipEmptyParts);
    config.cwd = m_cwdEdit->text();
    config.debuggerPath = m_debuggerEdit->text();
    if (config.name.isEmpty())
        config.name = m_configs.at(idx).name;
    m_configs[idx] = config;
    m_configCombo->setItemText(idx, config.name);
    m_configCombo->setItemData(idx, config.name);
}
