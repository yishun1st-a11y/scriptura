#ifndef GITDIFFWIDGET_H
#define GITDIFFWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>

class GitDiffWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GitDiffWidget(QWidget *parent = nullptr);
    void setDiff(const QString &diff);

private:
    QTextEdit *m_edit;
    QVBoxLayout *m_layout;
};

#endif // GITDIFFWIDGET_H
