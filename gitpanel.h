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

private:
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
};

#endif // GITPANEL_H
