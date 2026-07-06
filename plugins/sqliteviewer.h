#ifndef SQLITEVIEWER_H
#define SQLITEVIEWER_H

#include <QWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class SqliteViewerPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SqliteViewerPanel(QWidget *parent = nullptr);
    ~SqliteViewerPanel() override;

    void setDatabaseFile(const QString &filePath);
    void refresh();

signals:
    void tableDblClicked(const QString &tableName);
    void queryExecuted(const QString &sql, int rowCount);

private slots:
    void onOpenClicked();
    void onTableClicked(QTreeWidgetItem *item, int column);
    void onQueryReturnPressed();

private:
    bool openDatabase(const QString &filePath);
    void loadTables();
    void loadTableData(const QString &tableName);
    void showError(const QString &msg);

    QTreeWidget *m_tableList;
    QTableWidget *m_dataView;
    QLineEdit *m_queryEdit;
    QPushButton *m_openButton;
    QPushButton *m_refreshButton;
    QLineEdit *m_statusLabel;
    QString m_dbFile;
    void *m_db;
};

#endif // SQLITEVIEWER_H
