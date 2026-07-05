#ifndef DEBUGPANEL_H
#define DEBUGPANEL_H

#include <QWidget>
#include <QTreeView>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSplitter>
#include "dapclient.h"

class DebugPanel : public QWidget
{
    Q_OBJECT
public:
    explicit DebugPanel(QWidget *parent = nullptr);
    void setClient(DapClient *client);

    void setStack(const QList<DapClient::StackFrame> &frames);
    void addWatch(const QString &expr);
    void clearWatches();

signals:
    void frameActivated(int index);
    void watchRequested(const QString &expr);
    void evaluateRequested(const QString &expr);

private slots:
    void onStackActivated(const QModelIndex &index);
    void onScopeActivated(const QModelIndex &index);
    void onWatchReturnPressed();
    void onEvalReturnPressed();
    void onExpand(const QModelIndex &index);

private:
    QTreeView *m_stackTree;
    QTreeView *m_watchesTree;
    QTreeView *m_variablesTree;
    QTabWidget *m_tabs;
    QLineEdit *m_watchEdit;
    QLineEdit *m_evalEdit;
    DapClient *m_client;
};

#endif // DEBUGPANEL_H
