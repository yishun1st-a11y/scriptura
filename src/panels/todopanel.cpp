#include "todopanel.h"

#include <QRegularExpression>
#include <QFileInfo>

TodoPanel::TodoPanel(QWidget *parent)
    : QWidget(parent)
    , m_list(new QListWidget(this))
    , m_mainLayout(new QVBoxLayout(this))
{
    setObjectName("todoPanel");
    m_list->setAlternatingRowColors(true);
    m_mainLayout->addWidget(m_list);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);
    m_mainLayout->setSpacing(4);

    connect(m_list, &QListWidget::itemActivated, this, &TodoPanel::onItemActivated);
    connect(m_list, &QListWidget::itemDoubleClicked, this, &TodoPanel::onItemActivated);
}

TodoPanel::~TodoPanel()
{
}

void TodoPanel::parseDocument(const QString &text, const QString &fileName)
{
    m_list->clear();
    QRegularExpression re(R"((?:TODO|FIXME|XXX|BUG|HACK|NOTE)\s*[:\-]?\s*(.*))");
    QStringList lines = text.split("\n");
    for (int i = 0; i < lines.size(); ++i) {
        QRegularExpressionMatch match = re.match(lines[i]);
        if (match.hasMatch()) {
            QString label = QString("%1:%2  %3")
                .arg(fileName.isEmpty() ? "?" : QFileInfo(fileName).fileName())
                .arg(i + 1)
                .arg(match.captured(1).trimmed());
            QListWidgetItem *item = new QListWidgetItem(label);
            item->setData(Qt::UserRole, i); // 0-based line
            m_list->addItem(item);
        }
    }
    if (m_list->count() == 0)
        m_list->addItem(new QListWidgetItem(tr("(No TODO/FIXME comments found)")));
}

void TodoPanel::onItemActivated(QListWidgetItem *item)
{
    if (!item)
        return;
    bool ok = false;
    int line = item->data(Qt::UserRole).toInt(&ok);
    if (ok && line >= 0)
        emit todoActivated(line);
}
