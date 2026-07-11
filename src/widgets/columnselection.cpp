#include "columnselection.h"
#include <QTextBlock>
#include <QTextLayout>
#include <QTextFragment>

QList<QRectF> ColumnSelection::calcColumnRanges(const QTextCursor &cursor, const QPoint &startPos, const QPoint &endPos)
{
    QList<QRectF> ranges;
    QTextDocument *doc = cursor.document();
    if (!doc) return ranges;

    QTextBlock startBlock = doc->findBlock(cursor.selectionStart());
    QTextBlock endBlock = doc->findBlock(cursor.selectionEnd());
    
    int startLine = startBlock.blockNumber();
    int endLine = endBlock.blockNumber();
    
    int minX = qMin(startPos.x(), endPos.x());
    int maxX = qMax(startPos.x(), endPos.x());
    
    for (int line = startLine; line <= endLine; ++line) {
        QTextBlock block = doc->findBlockByNumber(line);
        if (!block.isValid()) continue;
        
        // Calculate block position using layout
        QTextLayout *layout = block.layout();
        qreal top = 0;
        qreal height = 0;
        
        if (layout) {
            // Get the position from the document layout
            for (int i = 0; i < line; ++i) {
                QTextBlock prevBlock = doc->findBlockByNumber(i);
                if (prevBlock.isValid() && prevBlock.layout()) {
                    // Accumulate heights of previous blocks
                }
            }
            top = block.position();
            height = block.layout()->boundingRect().height();
        }
        
        QRectF rangeRect(minX, top, maxX - minX, height);
        ranges.append(rangeRect);
    }
    
    return ranges;
}

QTextCursor ColumnSelection::createColumnCursor(const QTextCursor &baseCursor, const QRectF &range)
{
    QTextCursor cursor(baseCursor);
    QTextDocument *doc = cursor.document();
    if (!doc) return cursor;
    
    QTextBlock block = doc->findBlock(static_cast<int>(range.top()));
    if (!block.isValid()) return cursor;
    
    QTextLayout *layout = block.layout();
    if (!layout) return cursor;
    
    int startPos = block.position() + layout->lineAt(0).xToCursor(range.left());
    int endPos = block.position() + layout->lineAt(0).xToCursor(range.right());
    
    cursor.setPosition(startPos);
    cursor.setPosition(endPos, QTextCursor::KeepAnchor);
    
    return cursor;
}
