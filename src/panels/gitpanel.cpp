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
#include <QHeaderView>

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
    , m_diffButton(new QPushButton(tr("Diff"), this))
    , m_refreshButton(new QPushButton(tr("Refresh"), this))
{
    setObjectName("gitPanel");

    m_outputEdit->setReadOnly(true);
    m_outputEdit->setPlaceholderText(tr("Git output will appear here..."));

    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({tr("Status"), tr("Path")});
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
    stageActions->addWidget(m_diffButton);
    stageActions->addWidget(m_refreshButton);
    stageActions->addStretch();
    stageLayout->addLayout(stageActions);
    stageLayout->addWidget(m_stageView);

    // Branch tab
    m_tabs->addTab(m_branchWidget, tr("Branches"));

    // Diff tab
    m_tabs->addTab(m_diffWidget, tr("Diff"));

    // Merge tab
    m_tabs->addTab(m_mergeWidget, tr("Merge"));

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
    connect(m_diffButton, &QPushButton::clicked, this, &GitPanel::onDiffClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &GitPanel::refresh);
    connect(m_stageView, &QTreeView::doubleClicked, this, &GitPanel::onStageDoubleClicked);
    connect(m_branchWidget, &GitBranchWidget::checkoutRequested, this, &GitPanel::onBranchCheckout);
    connect(m_branchWidget, &GitBranchWidget::createRequested, this, &GitPanel::onBranchCreate);
    connect(m_branchWidget, &GitBranchWidget::deleteRequested, this, &GitPanel::onBranchDelete);
    connect(m_mergeWidget, &GitMergeWidget::acceptOurs, this, &GitPanel::onMergeAcceptOurs);
    connect(m_mergeWidget, &GitMergeWidget::acceptTheirs, this, &GitPanel::onMergeAcceptTheirs);
    connect(m_mergeWidget, &GitMergeWidget::acceptBoth, this, &GitPanel::onMergeAcceptBoth);
}

GitPanel::~GitPanel()
{
}

QString GitPanel::gitOutput(const QStringList &args)
{
    if (m_projectPath.isEmpty())
        return QString();
    QProcess git;
    git.setWorkingDirectory(m_projectPath);
    git.start("git", args);
    if (!git.waitForFinished(15000))
        return git.errorString();
    return QString::fromLocal8Bit(git.readAllStandardOutput())
         + QString::fromLocal8Bit(git.readAllStandardError());
}

void GitPanel::runGit(const QStringList &args, bool appendOutput)
{
    QString out = gitOutput(args);
    if (appendOutput) {
        QString existing = m_outputEdit->toPlainText();
        if (!existing.isEmpty())
            existing += "\n";
        m_outputEdit->setPlainText(existing + out);
    }
}

void GitPanel::setProjectPath(const QString &path)
{
    m_projectPath = path;
    refresh();
}

void GitPanel::setOutput(const QString &text)
{
    m_outputEdit->setPlainText(text);
    m_tabs->setCurrentIndex(4);
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
    m_tabs->setCurrentIndex(2);
}

void GitPanel::detectMergeConflicts()
{
    if (m_projectPath.isEmpty())
        return;
    QString conflicts = gitOutput({"diff", "--name-only", "--diff-filter=U"});
    QStringList files = conflicts.split("\n", Qt::SkipEmptyParts);
    files.removeAll(QString());
    if (files.isEmpty())
        return;

    QString path = m_projectPath + "/" + files.first();
    m_mergeFilePath = path;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QString content = QString::fromUtf8(f.readAll());
    f.close();

    QString ours, theirs, result;
    bool inOurs = false, inTheirs = false;
    for (const QString &line : content.split("\n")) {
        if (line.startsWith("<<<<<<<")) {
            inOurs = true;
        } else if (line.startsWith("=======") && inOurs) {
            inOurs = false;
            inTheirs = true;
        } else if (line.startsWith(">>>>>>>")) {
            inTheirs = false;
        } else if (inOurs) {
            ours += line + "\n";
            result += line + "\n";
        } else if (inTheirs) {
            theirs += line + "\n";
        } else {
            result += line + "\n";
        }
    }
    showMerge(ours, theirs, result);
}

void GitPanel::refresh()
{
    if (m_projectPath.isEmpty())
        return;

    // Populate staging view from git status --porcelain
    QString status = gitOutput({"status", "--porcelain"});
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(m_stageView->model());
    if (!model)
        model = new QStandardItemModel(this);
    model->removeRows(0, model->rowCount());
    for (const QString &line : status.split("\n", Qt::SkipEmptyParts)) {
        if (line.size() < 4)
            continue;
        QString staged = line.mid(0, 1).trimmed();
        QString unstaged = line.mid(1, 1).trimmed();
        QString path = line.mid(3);
        QString state = !staged.isEmpty() ? staged : unstaged;
        QList<QStandardItem*> row;
        row << new QStandardItem(state);
        row << new QStandardItem(path);
        model->appendRow(row);
    }
    m_stageView->setModel(model);
    m_stageView->setHeaderHidden(true);

    // Populate branches
    QString localOut = gitOutput({"branch", "--format=%(refname:short)"});
    QStringList local = localOut.split("\n", Qt::SkipEmptyParts);
    local.removeAll(QString());
    QString remoteOut = gitOutput({"branch", "-r", "--format=%(refname:short)"});
    QStringList remote = remoteOut.split("\n", Qt::SkipEmptyParts);
    remote.removeAll(QString());
    m_branchWidget->setBranches(local, remote);
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

QStringList GitPanel::selectedStagePaths() const
{
    QStringList paths;
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(m_stageView->model());
    if (!model)
        return paths;
    QModelIndexList idxs = m_stageView->selectionModel()->selectedRows(1);
    if (idxs.isEmpty())
        idxs = m_stageView->selectionModel()->selectedIndexes();
    for (const QModelIndex &idx : idxs) {
        QStandardItem *item = model->itemFromIndex(idx.sibling(idx.row(), 1));
        if (item && !item->text().isEmpty())
            paths.append(item->text());
    }
    return paths;
}

void GitPanel::onStageClicked()
{
    QStringList paths = selectedStagePaths();
    if (paths.isEmpty()) {
        runGit({"add", "-A"}, false);
    } else {
        for (const QString &p : paths)
            runGit({"add", "--", p}, false);
    }
    refresh();
}

void GitPanel::onUnstageClicked()
{
    QStringList paths = selectedStagePaths();
    if (paths.isEmpty()) {
        runGit({"restore", "--staged", "."}, false);
    } else {
        for (const QString &p : paths)
            runGit({"restore", "--staged", "--", p}, false);
    }
    refresh();
}

void GitPanel::onDiffClicked()
{
    QStringList paths = selectedStagePaths();
    QString diff = paths.isEmpty()
        ? gitOutput({"diff"})
        : gitOutput(QStringList({"diff", "--"}) + paths);
    setDiff(diff);
}

void GitPanel::onStageDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    onDiffClicked();
}

void GitPanel::onBranchCheckout(const QString &branch)
{
    if (branch.isEmpty())
        return;
    runGit({"checkout", branch});
    refresh();
}

void GitPanel::onBranchCreate(const QString &name)
{
    if (name.isEmpty())
        return;
    runGit({"checkout", "-b", name});
    refresh();
}

void GitPanel::onBranchDelete(const QString &branch)
{
    if (branch.isEmpty())
        return;
    runGit({"branch", "-d", branch});
    refresh();
}

void GitPanel::onMergeAcceptOurs()
{
    if (m_mergeFilePath.isEmpty() || !QFile::exists(m_mergeFilePath))
        return;
    QFile f(m_mergeFilePath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text))
        f.write(m_mergeWidget->resultText().toUtf8());
    runGit({"add", m_mergeFilePath}, false);
    m_outputEdit->append(tr("Merge result accepted (ours)."));
}

void GitPanel::onMergeAcceptTheirs()
{
    if (m_mergeFilePath.isEmpty() || !QFile::exists(m_mergeFilePath))
        return;
    QFile f(m_mergeFilePath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text))
        f.write(m_mergeWidget->resultText().toUtf8());
    runGit({"add", m_mergeFilePath}, false);
    m_outputEdit->append(tr("Merge result accepted (theirs)."));
}

void GitPanel::onMergeAcceptBoth()
{
    if (m_mergeFilePath.isEmpty() || !QFile::exists(m_mergeFilePath))
        return;
    QFile f(m_mergeFilePath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text))
        f.write(m_mergeWidget->resultText().toUtf8());
    runGit({"add", m_mergeFilePath}, false);
    m_outputEdit->append(tr("Merge result accepted (both)."));
}
