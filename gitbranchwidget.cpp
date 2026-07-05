#include "gitbranchwidget.h"

#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>

GitBranchWidget::GitBranchWidget(QWidget *parent)
    : QWidget(parent)
    , m_tree(new QTreeView(this))
    , m_layout(new QVBoxLayout(this))
    , m_checkoutBtn(new QPushButton(tr("Checkout"), this))
    , m_createBtn(new QPushButton(tr("Create"), this))
    , m_deleteBtn(new QPushButton(tr("Delete"), this))
{
    m_tree->setHeaderHidden(true);
    m_tree->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QWidget *header = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->addWidget(m_checkoutBtn);
    headerLayout->addWidget(m_createBtn);
    headerLayout->addWidget(m_deleteBtn);
    headerLayout->addStretch();

    m_layout->addWidget(header);
    m_layout->addWidget(m_tree);

    connect(m_checkoutBtn, &QPushButton::clicked, this, &GitBranchWidget::onCheckoutClicked);
    connect(m_createBtn, &QPushButton::clicked, this, &GitBranchWidget::onCreateClicked);
    connect(m_deleteBtn, &QPushButton::clicked, this, &GitBranchWidget::onDeleteClicked);
}

void GitBranchWidget::setBranches(const QStringList &local, const QStringList &remote)
{
    QStandardItemModel *model = new QStandardItemModel(this);
    QStandardItem *localRoot = new QStandardItem(tr("Local Branches"));
    localRoot->setEditable(false);
    for (const QString &branch : local) {
        QStandardItem *item = new QStandardItem(branch);
        item->setEditable(false);
        localRoot->appendRow(item);
    }
    model->appendRow(localRoot);

    if (!remote.isEmpty()) {
        QStandardItem *remoteRoot = new QStandardItem(tr("Remote Branches"));
        remoteRoot->setEditable(false);
        for (const QString &branch : remote) {
            QStandardItem *item = new QStandardItem(branch);
            item->setEditable(false);
            remoteRoot->appendRow(item);
        }
        model->appendRow(remoteRoot);
    }
    m_tree->setModel(model);
    m_tree->expandAll();
}

void GitBranchWidget::setOutput(const QString &text)
{
    Q_UNUSED(text)
}

void GitBranchWidget::onCheckoutClicked()
{
    QModelIndex index = m_tree->currentIndex();
    if (!index.isValid())
        return;
    QString branch = index.data().toString();
    emit checkoutRequested(branch);
}

void GitBranchWidget::onCreateClicked()
{
    bool ok = false;
    QString name = QInputDialog::getText(this, tr("Create Branch"), tr("Branch name:"), QLineEdit::Normal, QString(), &ok);
    if (ok && !name.isEmpty()) {
        emit createRequested(name);
    }
}

void GitBranchWidget::onDeleteClicked()
{
    QModelIndex index = m_tree->currentIndex();
    if (!index.isValid())
        return;
    QString branch = index.data().toString();
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Delete Branch"),
        tr("Delete branch '%1'?").arg(branch));
    if (reply == QMessageBox::Yes) {
        emit deleteRequested(branch);
    }
}
