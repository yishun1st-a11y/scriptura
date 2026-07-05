#include "gitdiffwidget.h"

#include <QApplication>
#include <QPalette>

GitDiffWidget::GitDiffWidget(QWidget *parent)
    : QWidget(parent)
    , m_edit(new QTextEdit(this))
    , m_layout(new QVBoxLayout(this))
{
    m_edit->setReadOnly(true);
    m_edit->setFontFamily(QStringLiteral("monospace"));
    m_edit->setLineWrapMode(QTextEdit::NoWrap);
    m_layout->addWidget(m_edit);
    m_layout->setContentsMargins(4, 4, 4, 4);
}

void GitDiffWidget::setDiff(const QString &diff)
{
    QString html = diff.toHtmlEscaped();
    html.replace("&quot;", "\"");
    html.replace("\n", "<br>");

    QPalette pal = qApp->palette();
    QString base = pal.color(QPalette::Base).name();
    QString text = pal.color(QPalette::Text).name();

    QString styled = QString(
        "<style>"
        "body { background-color: %1; color: %2; font-family: monospace; white-space: pre; }"
        ".diff-minus { color: #c0392b; }"
        ".diff-plus { color: #27ae60; }"
        ".diff-hdr { color: #8e44ad; }"
        "</style><body>"
    ).arg(base, text);

    QStringList lines = html.split("<br>");
    for (const QString &line : lines) {
        if (line.startsWith("-")) {
            styled += QString("<span class=\"diff-minus\">%1</span><br>").arg(line);
        } else if (line.startsWith("+")) {
            styled += QString("<span class=\"diff-plus\">%1</span><br>").arg(line);
        } else if (line.startsWith("@@") || line.startsWith("diff --git") || line.startsWith("index ")) {
            styled += QString("<span class=\"diff-hdr\">%1</span><br>").arg(line);
        } else {
            styled += line + "<br>";
        }
    }
    styled += "</body>";

    m_edit->setHtml(styled);
}
