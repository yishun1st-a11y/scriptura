#include "gitmergewidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

GitMergeWidget::GitMergeWidget(QWidget *parent)
    : QWidget(parent)
    , m_oursEdit(new QTextEdit(this))
    , m_theirsEdit(new QTextEdit(this))
    , m_resultEdit(new QTextEdit(this))
    , m_oursBtn(new QPushButton(tr("Accept Ours"), this))
    , m_theirsBtn(new QPushButton(tr("Accept Theirs"), this))
    , m_bothBtn(new QPushButton(tr("Accept Both"), this))
    , m_splitter(new QSplitter(Qt::Horizontal, this))
{
    m_oursEdit->setPlaceholderText(tr("Ours"));
    m_theirsEdit->setPlaceholderText(tr("Theirs"));
    m_resultEdit->setPlaceholderText(tr("Result"));
    m_resultEdit->setReadOnly(true);

    m_splitter->addWidget(m_oursEdit);
    m_splitter->addWidget(m_theirsEdit);
    m_splitter->addWidget(m_resultEdit);

    QWidget *actions = new QWidget(this);
    QHBoxLayout *actionsLayout = new QHBoxLayout(actions);
    actionsLayout->addWidget(m_oursBtn);
    actionsLayout->addWidget(m_theirsBtn);
    actionsLayout->addWidget(m_bothBtn);
    actionsLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_splitter);
    layout->addWidget(actions);

    connect(m_oursBtn, &QPushButton::clicked, this, &GitMergeWidget::onAcceptOurs);
    connect(m_theirsBtn, &QPushButton::clicked, this, &GitMergeWidget::onAcceptTheirs);
    connect(m_bothBtn, &QPushButton::clicked, this, &GitMergeWidget::onAcceptBoth);
}

void GitMergeWidget::setConflict(const QString &ours, const QString &theirs, const QString &result)
{
    m_oursEdit->setPlainText(ours);
    m_theirsEdit->setPlainText(theirs);
    m_resultEdit->setPlainText(result);
}

void GitMergeWidget::onAcceptOurs()
{
    m_resultEdit->setPlainText(m_oursEdit->toPlainText());
    emit acceptOurs();
}

void GitMergeWidget::onAcceptTheirs()
{
    m_resultEdit->setPlainText(m_theirsEdit->toPlainText());
    emit acceptTheirs();
}

void GitMergeWidget::onAcceptBoth()
{
    QString combined = m_oursEdit->toPlainText() + "\n" + m_theirsEdit->toPlainText();
    m_resultEdit->setPlainText(combined);
    emit acceptBoth();
}
