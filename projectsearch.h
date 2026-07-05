#ifndef PROJECTSEARCHPANEL_H
#define PROJECTSEARCHPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeView>
#include <QStandardItemModel>
#include <QProcess>
#include <QThread>

class ProjectSearchPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectSearchPanel(QWidget *parent = nullptr);
    ~ProjectSearchPanel();

    void search(const QString &term, const QString &rootPath);
    void clearResults();
    QString currentRootPath() const;

signals:
    void resultActivated(const QString &filePath, int line, int column);

private slots:
    void onSearchClicked();
    void onItemDoubleClicked(const QModelIndex &index);
    void onReturnPressed();

private:
    enum class Backend { Ripgrep, QtScanner };

    void searchWithRipgrep(const QString &term, const QString &rootPath);
    void searchWithQtScanner(const QString &term, const QString &rootPath);
    void appendResult(const QString &filePath, int line, int column, const QString &text);

    QLineEdit *m_searchEdit;
    QPushButton *m_searchBtn;
    QTreeView *m_resultsTree;
    QStandardItemModel *m_model;
    QProcess *m_process;
    QString m_currentRoot;
    bool m_running;
};

#endif // PROJECTSEARCHPANEL_H
