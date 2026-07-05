#include "multi-cursor.h"

MultiCursorManager::MultiCursorManager(QObject *parent)
    : QObject(parent)
{
}

void MultiCursorManager::addCursor(const QTextCursor &cursor)
{
    m_cursors.append(cursor);
    emit cursorsChanged();
}

void MultiCursorManager::removeLastCursor()
{
    if (!m_cursors.isEmpty()) {
        m_cursors.removeLast();
        emit cursorsChanged();
    }
}

void MultiCursorManager::clear()
{
    if (!m_cursors.isEmpty()) {
        m_cursors.clear();
        emit cursorsChanged();
    }
}

void MultiCursorManager::setCursors(const QList<QTextCursor> &cursors)
{
    m_cursors = cursors;
    emit cursorsChanged();
}
