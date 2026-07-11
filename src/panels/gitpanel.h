#ifndef GITPANEL_H
#define GITPANEL_H

#include <QWidget>
#include <QTabWidget>
#include <QTreeView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>

class GitDiffWidget;
class GitBranchWidget;
class GitMergeWidget;

class GitPanel : public QWidget
{
    Q_OBJECT
public:
    explicit GitPanel(QWidget *parent = nullptr);
    ~GitPanel();

    void setOutput(const QString &text);
    void setDiff(const QString &diff);
    void setBranches(const QStringList &local, const QStringList &remote);
    void showMerge(const QString &ours, const QString &theirs, const QString &result);
    void detectMergeConflicts();
    void setProjectPath(const QString &path);

signals:
    void commitRequested();
    void pushRequested();
    void stageRequested(const QString &path);
    void unstageRequested(const QString &path);
    void pullRequested();
    void fetchRequested();

private slots:
    void onCommitClicked();
    void onPushClicked();
    void onPullClicked();
    void onFetchClicked();
    void onStageClicked();
    void onUnstageClicked();
    void onDiffClicked();
    void onStageDoubleClicked(const QModelIndex &index);
    void onBranchCheckout(const QString &branch);
    void onBranchCreate(const QString &name);
    void onBranchDelete(const QString &branch);
    void onMergeAcceptOurs();
    void onMergeAcceptTheirs();
    void onMergeAcceptBoth();
    void refresh();

private:
    QString gitOutput(const QStringList &args);
    void runGit(const QStringList &args, bool appendOutput = true);
    QStringList selectedStagePaths() const;

    QTabWidget *m_tabs;
    QVBoxLayout *m_mainLayout;
    QTextEdit *m_outputEdit;
    GitDiffWidget *m_diffWidget;
    GitBranchWidget *m_branchWidget;
    GitMergeWidget *m_mergeWidget;
    QTreeView *m_stageView;
    QPushButton *m_commitButton;
    QPushButton *m_pushButton;
    QPushButton *m_pullButton;
    QPushButton *m_fetchButton;
    QPushButton *m_stageButton;
    QPushButton *m_unstageButton;
    QPushButton *m_diffButton;
    QPushButton *m_refreshButton;
    QString m_projectPath;
    QString m_mergeFilePath;
};

#endif // GITPANEL_H
