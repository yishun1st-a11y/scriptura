#ifndef PROBLEMPANEL_H
#define PROBLEMPANEL_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QTabBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include "lspclient.h"

class ProblemPanel : public QWidget
{
    Q_OBJECT
public:
    enum Filter { All = 0, Errors = 1, Warnings = 2, Info = 3 };

    explicit ProblemPanel(QWidget *parent = nullptr);
    ~ProblemPanel();

    void setProblems(const QString &fileUri, const QList<LspClient::Diagnostic> &diagnostics);
    void clearProblems(const QString &fileUri);
    void clearAll();
    int problemCount(Filter filter = All) const;

    void setCurrentFile(const QString &fileUri);
    QString currentFile() const { return m_currentFile; }

signals:
    void problemActivated(const QString &fileUri, int line, int column);
    void filterChanged(Filter filter);

public slots:
    void setFilter(Filter filter);
    void onItemActivated(QTreeWidgetItem *item, int column);

private slots:
    void onFilterTabChanged(int index);
    void onCloseClicked();
    void onProblemsChanged(const QString &uri, const QList<LspClient::Diagnostic> &diags);

private:
    struct ProblemItem {
        QString fileUri;
        int line;
        int column;
        LspClient::Diagnostic::Severity severity;
        QString message;
        QString source;
    };

    void rebuildTree();
    void addProblemItem(const ProblemItem &item);
    QTreeWidgetItem *createTreeItem(const ProblemItem &item) const;
    QString severityIcon(LspClient::Diagnostic::Severity severity) const;
    QString severityText(LspClient::Diagnostic::Severity severity) const;
    QColor severityColor(LspClient::Diagnostic::Severity severity) const;

    QTreeWidget *m_treeWidget;
    QTabBar *m_filterTabs;
    QPushButton *m_closeButton;
    QLabel *m_countLabel;
    QVBoxLayout *m_mainLayout;

    QMap<QString, QList<LspClient::Diagnostic>> m_allProblems;
    QList<ProblemItem> m_filteredProblems;
    Filter m_currentFilter;
    QString m_currentFile;
};

#endif // PROBLEMPANEL_H