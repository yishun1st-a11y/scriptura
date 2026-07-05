#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDirIterator>
#include <QTextStream>
#include <QFileInfo>
#include <QApplication>
#include <QStandardPaths>

#include "projectsearch.h"

ProjectSearchPanel::ProjectSearchPanel(QWidget *parent)
    : QWidget(parent)
    , m_searchEdit(new QLineEdit(this))
    , m_searchBtn(new QPushButton(tr("Search"), this))
    , m_resultsTree(new QTreeView(this))
    , m_model(new QStandardItemModel(this))
    , m_process(new QProcess(this))
    , m_running(false)
{
    setObjectName("projectSearchPanel");

    m_searchEdit->setPlaceholderText(tr("Search in project..."));
    m_searchEdit->setClearButtonEnabled(true);

    m_resultsTree->setModel(m_model);
    m_resultsTree->setHeaderHidden(true);
    m_resultsTree->setAlternatingRowColors(true);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(m_searchEdit, 1);
    topLayout->addWidget(m_searchBtn);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(topLayout);
    layout->addWidget(m_resultsTree);

    connect(m_searchBtn, &QPushButton::clicked, this, &ProjectSearchPanel::onSearchClicked);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &ProjectSearchPanel::onReturnPressed);
    connect(m_resultsTree, &QTreeView::doubleClicked, this, &ProjectSearchPanel::onItemDoubleClicked);
    connect(m_process, &QProcess::readyReadStandardOutput, this, [this]() {
        QByteArray data = m_process->readAllStandardOutput();
        QString text = QString::fromLocal8Bit(data);
        for (const QString &line : text.split('\n')) {
            if (line.isEmpty())
                continue;
            QStringList parts = line.split(':');
            if (parts.size() >= 3) {
                QString file = parts[0];
                bool lineOk = false;
                int lineNum = parts[1].toInt(&lineOk);
                int colNum = parts.size() > 2 ? parts[2].toInt(&lineOk) : 1;
                QString content = parts.mid(3).join(':');
                appendResult(file, lineNum, colNum, content);
            }
        }
    });
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int, QProcess::ExitStatus) {
                m_running = false;
            });
}

ProjectSearchPanel::~ProjectSearchPanel()
{
    if (m_process->state() == QProcess::Running)
        m_process->kill();
}

void ProjectSearchPanel::search(const QString &term, const QString &rootPath)
{
    if (term.isEmpty() || rootPath.isEmpty())
        return;

    m_currentRoot = rootPath;
    clearResults();

    if (QStandardPaths::findExecutable("rg").isEmpty()) {
        searchWithQtScanner(term, rootPath);
    } else {
        searchWithRipgrep(term, rootPath);
    }
}

void ProjectSearchPanel::clearResults()
{
    m_model->clear();
    m_model->setHorizontalHeaderLabels(QStringList() << tr("Results"));
    if (m_process->state() == QProcess::Running)
        m_process->kill();
    m_running = false;
}

QString ProjectSearchPanel::currentRootPath() const
{
    return m_currentRoot;
}

void ProjectSearchPanel::onSearchClicked()
{
    QString term = m_searchEdit->text();
    if (!term.isEmpty() && !m_currentRoot.isEmpty())
        search(term, m_currentRoot);
}

void ProjectSearchPanel::onItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString filePath = m_model->data(index, Qt::UserRole).toString();
    int line = m_model->data(index, Qt::UserRole + 1).toInt();
    int column = m_model->data(index, Qt::UserRole + 2).toInt();

    if (!filePath.isEmpty())
        emit resultActivated(filePath, line, column);
}

void ProjectSearchPanel::onReturnPressed()
{
    onSearchClicked();
}

void ProjectSearchPanel::searchWithRipgrep(const QString &term, const QString &rootPath)
{
    m_running = true;
    m_process->start("rg", {"--no-heading", "--line-number", "--with-filename", "--smart-case", term, rootPath});
}

void ProjectSearchPanel::searchWithQtScanner(const QString &term, const QString &rootPath)
{
    QDirIterator iterator(rootPath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QStringList textExtensions = {"txt", "cpp", "c", "h", "hpp", "py", "js", "ts", "java", "rs", "go", "sh", "html", "css", "md", "json", "xml", "yaml", "yml"};

    while (iterator.hasNext()) {
        iterator.next();
        QString path = iterator.filePath();
        QFileInfo info(path);
        if (!info.isFile())
            continue;
        if (info.size() > 10 * 1024 * 1024)
            continue;
        if (!textExtensions.contains(info.suffix().toLower()))
            continue;

        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QTextStream in(&file);
        int lineNum = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            lineNum++;
            if (line.contains(term, Qt::CaseInsensitive)) {
                int col = line.indexOf(term, 0, Qt::CaseInsensitive);
                appendResult(path, lineNum, col, line);
            }
        }
    }
}

void ProjectSearchPanel::appendResult(const QString &filePath, int line, int column, const QString &text)
{
    QList<QStandardItem*> items;
    QString label = QString("%1:%2:%3: %4").arg(filePath).arg(line).arg(column).arg(text.trimmed());
    QStandardItem *item = new QStandardItem(label);
    item->setData(filePath, Qt::UserRole);
    item->setData(line, Qt::UserRole + 1);
    item->setData(column, Qt::UserRole + 2);
    items.append(item);
    m_model->appendRow(items);
}
