#ifndef MULTI_CURSOR_H
#define MULTI_CURSOR_H

#include <QObject>
#include <QTextCursor>
#include <QList>

class MultiCursorManager : public QObject
{
    Q_OBJECT
public:
    explicit MultiCursorManager(QObject *parent = nullptr);

    void addCursor(const QTextCursor &cursor);
    void removeLastCursor();
    void clear();
    QList<QTextCursor> cursors() const { return m_cursors; }
    void setCursors(const QList<QTextCursor> &cursors);
    bool hasCursors() const { return !m_cursors.isEmpty(); }
    int cursorCount() const { return m_cursors.size(); }

signals:
    void cursorsChanged();

private:
    QList<QTextCursor> m_cursors;
};

#endif // MULTI_CURSOR_H
