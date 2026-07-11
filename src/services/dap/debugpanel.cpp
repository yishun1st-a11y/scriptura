#include "debugpanel.h"
#include "dapclient.h"

#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>

DebugPanel::DebugPanel(QWidget *parent)
    : QWidget(parent)
    , m_stackTree(new QTreeView(this))
    , m_watchesTree(new QTreeView(this))
    , m_variablesTree(new QTreeView(this))
    , m_tabs(new QTabWidget(this))
    , m_watchEdit(new QLineEdit(this))
    , m_evalEdit(new QLineEdit(this))
    , m_client(nullptr)
{
    m_stackTree->setHeaderHidden(true);
    m_stackTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_watchesTree->setHeaderHidden(true);
    m_variablesTree->setHeaderHidden(true);

    m_tabs->addTab(m_stackTree, tr("Call Stack"));
    m_tabs->addTab(m_watchesTree, tr("Watches"));
    m_tabs->addTab(m_variablesTree, tr("Variables"));

    m_watchEdit->setPlaceholderText(tr("Add watch expression..."));
    m_evalEdit->setPlaceholderText(tr("Evaluate expression..."));

    QWidget *inputArea = new QWidget(this);
    QHBoxLayout *inputLayout = new QHBoxLayout(inputArea);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->addWidget(m_watchEdit);
    inputLayout->addWidget(m_evalEdit);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_tabs);
    layout->addWidget(inputArea);

    connect(m_stackTree, &QTreeView::clicked, this, &DebugPanel::onStackActivated);
    connect(m_variablesTree, &QTreeView::clicked, this, &DebugPanel::onExpand);
    connect(m_watchEdit, &QLineEdit::returnPressed, this, &DebugPanel::onWatchReturnPressed);
    connect(m_evalEdit, &QLineEdit::returnPressed, this, &DebugPanel::onEvalReturnPressed);
}

void DebugPanel::setClient(DapClient *client)
{
    m_client = client;
}

void DebugPanel::setStack(const QList<DapClient::StackFrame> &frames)
{
    QStandardItemModel *model = new QStandardItemModel(this);
    for (const DapClient::StackFrame &frame : frames) {
        QStandardItem *item = new QStandardItem(QString("%1: %2").arg(frame.source.name).arg(frame.name));
        item->setData(frame.id, Qt::UserRole);
        model->appendRow(item);
    }
    m_stackTree->setModel(model);
}

void DebugPanel::addVariables(const QList<DapClient::Variable> &variables)
{
    if (!m_variablesTree->model()) {
        QStandardItemModel *model = new QStandardItemModel(this);
        m_variablesTree->setModel(model);
    }
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(m_variablesTree->model());
    if (!model) return;
    for (const DapClient::Variable &v : variables) {
        QString label = QString("%1 = %2").arg(v.name, v.value);
        if (!v.type.isEmpty())
            label += QString(" (%1)").arg(v.type);
        QStandardItem *item = new QStandardItem(label);
        item->setData(v.variablesReference, Qt::UserRole);
        model->appendRow(item);
    }
}

void DebugPanel::clearVariables()
{
    if (QStandardItemModel *model = qobject_cast<QStandardItemModel*>(m_variablesTree->model()))
        model->clear();
}

void DebugPanel::showEvaluation(const QString &expression, const QString &result)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(m_variablesTree->model());
    if (!model) {
        model = new QStandardItemModel(this);
        m_variablesTree->setModel(model);
    }
    model->appendRow(new QStandardItem(QString("%1 = %2").arg(expression, result)));
    emit evaluationContextReceived(expression, result, "console");
}

void DebugPanel::showWatchEvaluation(const QString &expression, const QString &result)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(m_watchesTree->model());
    if (!model) {
        model = new QStandardItemModel(this);
        m_watchesTree->setModel(model);
    }
    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *item = model->item(i);
        if (item && item->text() == expression) {
            item->setText(QString("%1 = %2").arg(expression, result));
            return;
        }
    }
    model->appendRow(new QStandardItem(QString("%1 = %2").arg(expression, result)));
}

void DebugPanel::addWatch(const QString &expr)
{
    if (!m_watchesTree->model()) {
        QStandardItemModel *model = new QStandardItemModel(this);
        m_watchesTree->setModel(model);
    }
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(m_watchesTree->model());
    if (!model) return;
    QStandardItem *item = new QStandardItem(expr);
    model->appendRow(item);
}

void DebugPanel::clearWatches()
{
    if (QStandardItemModel *model = qobject_cast<QStandardItemModel*>(m_watchesTree->model()))
        model->clear();
}

void DebugPanel::onStackActivated(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    bool ok = false;
    int frameId = index.data(Qt::UserRole).toInt(&ok);
    if (ok)
        emit frameActivated(frameId);
}

void DebugPanel::onExpand(const QModelIndex &index)
{
    if (!index.isValid() || !m_client)
        return;
    bool ok = false;
    int ref = index.data(Qt::UserRole).toInt(&ok);
    if (ok && ref > 0)
        m_client->variables(ref);
}

void DebugPanel::onWatchReturnPressed()
{
    QString expr = m_watchEdit->text().trimmed();
    if (!expr.isEmpty()) {
        addWatch(expr);
        emit watchRequested(expr);
        m_watchEdit->clear();
    }
}

void DebugPanel::onEvalReturnPressed()
{
    QString expr = m_evalEdit->text().trimmed();
    if (!expr.isEmpty()) {
        emit evaluateRequested(expr);
        m_evalEdit->clear();
    }
}
