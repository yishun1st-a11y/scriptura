#ifndef COMMANDPALETTE_H
#define COMMANDPALETTE_H

#include <QDialog>
#include <QLineEdit>
#include <QListView>
#include <QStringListModel>
#include <QStyledItemDelegate>
#include <functional>

struct Command {
    QString id;
    QString label;
    QString shortcut;
    std::function<void()> action;
};

class CommandPalette : public QDialog
{
    Q_OBJECT
public:
    explicit CommandPalette(QWidget *parent = nullptr);
    void registerCommand(const Command &cmd);
    void execute();

signals:
    void commandInvoked(const QString &commandId);

private slots:
    void onTextChanged(const QString &text);
    void onActivated(const QModelIndex &index);

private:
    int fuzzyScore(const QString &pattern, const QString &text) const;

    QLineEdit *m_lineEdit;
    QListView *m_listView;
    QStringListModel *m_model;
    QList<Command> m_commands;
    QList<Command> m_filtered;
};

#endif // COMMANDPALETTE_H
