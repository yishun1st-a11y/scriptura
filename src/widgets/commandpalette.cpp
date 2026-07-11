#include "commandpalette.h"

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QApplication>
#include <QPalette>

CommandPalette::CommandPalette(QWidget *parent)
    : QDialog(parent)
    , m_lineEdit(new QLineEdit(this))
    , m_listView(new QListView(this))
    , m_model(new QStringListModel(this))
{
    setWindowFlags(windowFlags() | Qt::Popup | Qt::FramelessWindowHint);
    setModal(true);
    setObjectName("commandPalette");

    m_listView->setModel(m_model);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listView->setUniformItemSizes(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_listView);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(4);

    resize(500, 400);

    connect(m_lineEdit, &QLineEdit::textChanged, this, &CommandPalette::onTextChanged);
    connect(m_lineEdit, &QLineEdit::returnPressed, this, [this]() {
        onActivated(m_listView->currentIndex());
    });
    connect(m_listView, &QListView::doubleClicked, this, &CommandPalette::onActivated);
    connect(m_listView, &QListView::clicked, this, &CommandPalette::onActivated);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, pal.color(QPalette::Base));
    setPalette(pal);
}

void CommandPalette::registerCommand(const Command &cmd)
{
    m_commands.append(cmd);
}

void CommandPalette::execute()
{
    m_lineEdit->clear();
    m_filtered = m_commands;
    QStringList labels;
    for (const Command &cmd : m_filtered)
        labels << cmd.label;
    m_model->setStringList(labels);
    m_listView->setCurrentIndex(m_model->index(0, 0));
    m_lineEdit->setFocus();
    QDialog::exec();
}

void CommandPalette::onTextChanged(const QString &text)
{
    m_filtered.clear();
    QString pattern = text.trimmed().toLower();

    for (const Command &cmd : m_commands) {
        if (pattern.isEmpty() || fuzzyScore(pattern, cmd.label.toLower()) > 0) {
            m_filtered.append(cmd);
        }
    }

    QStringList labels;
    for (const Command &cmd : m_filtered)
        labels << cmd.label;
    m_model->setStringList(labels);
    if (m_listView->currentIndex().row() >= m_filtered.size() && !m_filtered.isEmpty())
        m_listView->setCurrentIndex(m_model->index(0, 0));
}

void CommandPalette::onActivated(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= m_filtered.size())
        return;

    const Command &cmd = m_filtered[index.row()];
    if (cmd.action)
        cmd.action();
    emit commandInvoked(cmd.id);
    accept();
}

int CommandPalette::fuzzyScore(const QString &pattern, const QString &text) const
{
    int pi = 0;
    int score = 0;
    int lastMatchPos = -1;
    for (int ti = 0; ti < text.size() && pi < pattern.size(); ++ti) {
        if (text[ti] == pattern[pi]) {
            score += (lastMatchPos == ti - 1) ? 2 : 1;
            lastMatchPos = ti;
            pi++;
        }
    }
    return pi == pattern.size() ? score : 0;
}
