#ifndef GITMERGEWIDGET_H
#define GITMERGEWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QSplitter>
#include <QPushButton>

class GitMergeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GitMergeWidget(QWidget *parent = nullptr);
    void setConflict(const QString &ours, const QString &theirs, const QString &result);
    QString resultText() const { return m_resultEdit->toPlainText(); }

signals:
    void acceptOurs();
    void acceptTheirs();
    void acceptBoth();

private slots:
    void onAcceptOurs();
    void onAcceptTheirs();
    void onAcceptBoth();

private:
    QTextEdit *m_oursEdit;
    QTextEdit *m_theirsEdit;
    QTextEdit *m_resultEdit;
    QPushButton *m_oursBtn;
    QPushButton *m_theirsBtn;
    QPushButton *m_bothBtn;
    QSplitter *m_splitter;
};

#endif // GITMERGEWIDGET_H
