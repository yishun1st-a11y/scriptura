#ifndef GITBRANCHWIDGET_H
#define GITBRANCHWIDGET_H

#include <QWidget>
#include <QTreeView>
#include <QVBoxLayout>
#include <QPushButton>

class GitBranchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GitBranchWidget(QWidget *parent = nullptr);
    void setBranches(const QStringList &local, const QStringList &remote);
    void setOutput(const QString &text);

signals:
    void checkoutRequested(const QString &branch);
    void createRequested(const QString &name);
    void deleteRequested(const QString &branch);

private slots:
    void onCheckoutClicked();
    void onCreateClicked();
    void onDeleteClicked();

private:
    QTreeView *m_tree;
    QVBoxLayout *m_layout;
    QPushButton *m_checkoutBtn;
    QPushButton *m_createBtn;
    QPushButton *m_deleteBtn;
};

#endif // GITBRANCHWIDGET_H
