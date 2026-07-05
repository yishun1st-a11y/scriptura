#include "gitpanel.h"
#include "gitdiffwidget.h"
#include "gitbranchwidget.h"
#include "gitmergewidget.h"

#include <QApplication>
#include <QProcess>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItemModel>

GitPanel::GitPanel(QWidget *parent)
    : QWidget(parent)
    , m_tabs(new QTabWidget(this))
    , m_mainLayout(new QVBoxLayout(this))
    , m_outputEdit(new QTextEdit(this))
    , m_diffWidget(new GitDiffWidget(this))
    , m_branchWidget(new GitBranchWidget(this))
    , m_mergeWidget(new GitMergeWidget(this))
    , m_stageView(new QTreeView(this))
    , m_commitButton(new QPushButton(tr("Commit"), this))
    , m_pushButton(new QPushButton(tr("Push"), this))
    , m_pullButton(new QPushButton(tr("Pull"), this))
    , m_fetchButton(new QPushButton(tr("Fetch"), this))
    , m_stageButton(new QPushButton(tr("Stage"), this))
    , m_unstageButton(new QPushButton(tr("Unstage"), this))
{
    setObjectName("gitPanel");

    m_outputEdit->setReadOnly(true);
    m_outputEdit->setPlaceholderText(tr("Git output will appear here..."));

    QStringList columns = {tr("Status"), tr("Path")};
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(columns);
    m_stageView->setModel(model);
    m_stageView->setHeaderHidden(true);

    // Output tab
    QWidget *outputTab = new QWidget(this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputTab);
    outputLayout->addWidget(m_outputEdit);

    // Staging tab
    QWidget *stageTab = new QWidget(this);
    QVBoxLayout *stageLayout = new QVBoxLayout(stageTab);
    QHBoxLayout *stageActions = new QHBoxLayout();
    stageActions->addWidget(m_stageButton);
    stageActions->addWidget(m_unstageButton);
    stageActions->addStretch();
    stageLayout->addLayout(stageActions);
    stageLayout->addWidget(m_stageView);

    // Branch tab
    m_tabs->addTab(m_branchWidget, tr("Branches"));

    // Diff tab
    m_tabs->addTab(m_diffWidget, tr("Diff"));

    // Merge tab
    m_tabs->addTab(m_mergeWidget, tr("Merge"));
    m_tabs->setTabEnabled(3, false);

    // Output tab
    m_tabs->addTab(outputTab, tr("Output"));

    // Staging tab
    m_tabs->addTab(stageTab, tr("Staging"));

    QWidget *header = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(4, 4, 4, 4);
    headerLayout->setSpacing(4);
    headerLayout->addWidget(m_commitButton);
    headerLayout->addWidget(m_pushButton);
    headerLayout->addWidget(m_pullButton);
    headerLayout->addWidget(m_fetchButton);
    headerLayout->addStretch();

    m_mainLayout->addWidget(header);
    m_mainLayout->addWidget(m_tabs);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);
    m_mainLayout->setSpacing(4);

    connect(m_commitButton, &QPushButton::clicked, this, &GitPanel::onCommitClicked);
    connect(m_pushButton, &QPushButton::clicked, this, &GitPanel::onPushClicked);
    connect(m_pullButton, &QPushButton::clicked, this, &GitPanel::onPullClicked);
    connect(m_fetchButton, &QPushButton::clicked, this, &GitPanel::onFetchClicked);
    connect(m_stageButton, &QPushButton::clicked, this, &GitPanel::onStageClicked);
    connect(m_unstageButton, &QPushButton::clicked, this, &GitPanel::onUnstageClicked);
    connect(m_branchWidget, &GitBranchWidget::checkoutRequested, this, [](const QString &b) { qDebug() << "Checkout:" << b; });
    connect(m_branchWidget, &GitBranchWidget::createRequested, this, [](const QString &n) { qDebug() << "Create branch:" << n; });
    connect(m_branchWidget, &GitBranchWidget::deleteRequested, this, [](const QString &b) { qDebug() << "Delete branch:" << b; });
}

GitPanel::~GitPanel()
{
}

void GitPanel::setOutput(const QString &text)
{
    m_outputEdit->setPlainText(text);
    m_tabs->setCurrentIndex(3);
}

void GitPanel::setDiff(const QString &diff)
{
    m_diffWidget->setDiff(diff);
    m_tabs->setCurrentIndex(1);
}

void GitPanel::setBranches(const QStringList &local, const QStringList &remote)
{
    m_branchWidget->setBranches(local, remote);
    m_tabs->setCurrentIndex(0);
}

void GitPanel::showMerge(const QString &ours, const QString &theirs, const QString &result)
{
    m_mergeWidget->setConflict(ours, theirs, result);
    m_tabs->setTabEnabled(3, true);
    m_tabs->setCurrentIndex(3);
}

void GitPanel::onCommitClicked()
{
    emit commitRequested();
}

void GitPanel::onPushClicked()
{
    emit pushRequested();
}

void GitPanel::onPullClicked()
{
    emit pullRequested();
}

void GitPanel::onFetchClicked()
{
    emit fetchRequested();
}

void GitPanel::onStageClicked()
{
}

void GitPanel::onUnstageClicked()
{
}
