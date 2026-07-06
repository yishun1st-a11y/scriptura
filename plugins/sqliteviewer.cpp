#include "sqliteviewer.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QApplication>
#include <QHeaderView>
#include <QSplitter>

SqliteViewerPanel::SqliteViewerPanel(QWidget *parent)
    : QWidget(parent)
    , m_tableList(new QTreeWidget(this))
    , m_dataView(new QTableWidget(this))
    , m_queryEdit(new QLineEdit(this))
    , m_openButton(new QPushButton(tr("Open SQLite..."), this))
    , m_refreshButton(new QPushButton(tr("Refresh"), this))
    , m_statusLabel(new QLineEdit(this))
    , m_db(nullptr)
{
    m_tableList->setHeaderLabels({tr("Tables")});
    m_tableList->setColumnCount(1);
    m_tableList->header()->setSectionResizeMode(QHeaderView::Stretch);

    m_dataView->horizontalHeader()->setStretchLastSection(true);
    m_dataView->verticalHeader()->setVisible(false);

    m_queryEdit->setPlaceholderText(tr("SELECT * FROM ..."));
    m_statusLabel->setReadOnly(true);
    m_statusLabel->setClearButtonEnabled(true);
    m_statusLabel->setPlaceholderText(tr("No database opened"));

    QHBoxLayout *topBar = new QHBoxLayout();
    topBar->addWidget(m_openButton);
    topBar->addWidget(m_refreshButton);
    topBar->addWidget(m_queryEdit, 1);
    topBar->addWidget(m_statusLabel, 1);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_tableList);
    splitter->addWidget(m_dataView);
    splitter->setSizes({200, 400});

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topBar);
    mainLayout->addWidget(splitter);
    setLayout(mainLayout);

    connect(m_openButton, &QPushButton::clicked, this, &SqliteViewerPanel::onOpenClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &SqliteViewerPanel::refresh);
    connect(m_queryEdit, &QLineEdit::returnPressed, this, &SqliteViewerPanel::onQueryReturnPressed);
    connect(m_tableList, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item, int) {
        if (item)
            loadTableData(item->text(0));
    });
}

SqliteViewerPanel::~SqliteViewerPanel()
{
    if (m_db) {
        QSqlDatabase db = QSqlDatabase::database("sqliteviewer");
        if (db.isOpen())
            db.close();
        QSqlDatabase::removeDatabase("sqliteviewer");
    }
}

void SqliteViewerPanel::setDatabaseFile(const QString &filePath)
{
    m_dbFile = filePath;
    openDatabase(filePath);
    refresh();
}

void SqliteViewerPanel::refresh()
{
    if (m_dbFile.isEmpty()) {
        m_tableList->clear();
        m_dataView->setRowCount(0);
        return;
    }
    openDatabase(m_dbFile);
    loadTables();
}

void SqliteViewerPanel::onOpenClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open SQLite Database"), QString(), tr("SQLite Databases (*.sqlite *.db *.sqlite3);;All Files (*)"));
    if (filePath.isEmpty())
        return;
    setDatabaseFile(filePath);
}

void SqliteViewerPanel::onTableClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    if (item)
        loadTableData(item->text(0));
}

void SqliteViewerPanel::onQueryReturnPressed()
{
    if (!m_dbFile.isEmpty())
        loadTableData(m_queryEdit->text());
}

bool SqliteViewerPanel::openDatabase(const QString &filePath)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "sqliteviewer");
    db.setDatabaseName(filePath);
    if (!db.open()) {
        showError(db.lastError().text());
        return false;
    }
    m_dbFile = filePath;
    m_statusLabel->setText(QString("%1 | tables loaded").arg(QFileInfo(filePath).fileName()));
    return true;
}

void SqliteViewerPanel::loadTables()
{
    m_tableList->clear();
    if (m_dbFile.isEmpty())
        return;

    QSqlDatabase db = QSqlDatabase::database("sqliteviewer");
    if (!db.isOpen())
        return;

    QSqlQuery query(db);
    query.exec("SELECT name FROM sqlite_master WHERE type='table' OR type='view' ORDER BY name");
    while (query.next()) {
        QString name = query.value(0).toString();
        QTreeWidgetItem *item = new QTreeWidgetItem({name});
        m_tableList->addTopLevelItem(item);
    }
}

void SqliteViewerPanel::loadTableData(const QString &tableOrQuery)
{
    QSqlDatabase db = QSqlDatabase::database("sqliteviewer");
    if (!db.isOpen() || tableOrQuery.isEmpty())
        return;

    QString sql = tableOrQuery.trimmed();
    if (!sql.startsWith("SELECT", Qt::CaseInsensitive)) {
        sql = "SELECT * FROM " + sql;
    }

    QSqlQuery query(db);
    if (!query.exec(sql)) {
        showError(query.lastError().text());
        return;
    }

    QList<QSqlRecord> rows;
    while (query.next()) {
        rows.append(query.record());
    }

    if (rows.isEmpty()) {
        m_dataView->setRowCount(0);
        m_dataView->setColumnCount(0);
        return;
    }

    QSqlRecord first = rows.first();
    m_dataView->setColumnCount(first.count());
    QStringList headers;
    for (int i = 0; i < first.count(); ++i) {
        headers.append(first.fieldName(i));
    }
    m_dataView->setHorizontalHeaderLabels(headers);
    m_dataView->setRowCount(rows.size());

    for (int r = 0; r < rows.size(); ++r) {
        const QSqlRecord &rec = rows[r];
        for (int c = 0; c < rec.count(); ++c) {
            QVariant val = rec.value(c);
            m_dataView->setItem(r, c, new QTableWidgetItem(val.isNull() ? QString("NULL") : val.toString()));
        }
    }

    emit queryExecuted(sql, rows.size());
    m_statusLabel->setText(QString("%1 rows").arg(rows.size()));
}

void SqliteViewerPanel::showError(const QString &msg)
{
    m_statusLabel->setText(tr("Error: %1").arg(msg));
}
