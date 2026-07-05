#ifndef COLUMNSELECTION_H
#define COLUMNSELECTION_H

#include <QRectF>
#include <QPointF>
#include <QTextCursor>
#include <QList>

class ColumnSelection
{
public:
    static QList<QRectF> calcColumnRanges(const QTextCursor &cursor, const QPoint &startPos, const QPoint &endPos);
    static QTextCursor createColumnCursor(const QTextCursor &baseCursor, const QRectF &range);
};

#endif // COLUMNSELECTION_H
