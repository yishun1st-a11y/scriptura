#include "lengthprefixedframer.h"

LengthPrefixedFramer::LengthPrefixedFramer(QObject *parent)
    : QObject(parent)
{
}

void LengthPrefixedFramer::append(const QByteArray &data)
{
    QMutexLocker locker(&m_mutex);
    m_buffer.append(data);
}

bool LengthPrefixedFramer::canReadMessage() const
{
    QMutexLocker locker(&m_mutex);
    static const QByteArray headerEnd = "\r\n\r\n";
    int headerEndPos = m_buffer.indexOf(headerEnd);
    if (headerEndPos == -1)
        return false;

    QByteArray header = m_buffer.left(headerEndPos);
    QRegularExpression re("Content-Length:\\s*(\\d+)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(header);
    if (!match.hasMatch())
        return false;

    int contentLength = match.captured(1).toInt();
    return m_buffer.size() >= headerEndPos + headerEnd.size() + contentLength;
}

QByteArray LengthPrefixedFramer::readMessage()
{
    QMutexLocker locker(&m_mutex);
    static const QByteArray headerEnd = "\r\n\r\n";
    int headerEndPos = m_buffer.indexOf(headerEnd);
    if (headerEndPos == -1)
        return QByteArray();

    QByteArray header = m_buffer.left(headerEndPos);
    QRegularExpression re("Content-Length:\\s*(\\d+)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(header);
    if (!match.hasMatch()) {
        m_buffer.remove(0, headerEndPos + headerEnd.size());
        return QByteArray();
    }

    int contentLength = match.captured(1).toInt();
    int totalSize = headerEndPos + headerEnd.size() + contentLength;
    if (m_buffer.size() < totalSize)
        return QByteArray();

    QByteArray msg = m_buffer.mid(headerEndPos + headerEnd.size(), contentLength);
    m_buffer.remove(0, totalSize);
    return msg;
}

QByteArray LengthPrefixedFramer::frame(const QByteArray &payload)
{
    QByteArray header = QString("Content-Length: %1\r\n\r\n").arg(payload.size()).toUtf8();
    return header + payload;
}

void LengthPrefixedFramer::clear()
{
    QMutexLocker locker(&m_mutex);
    m_buffer.clear();
}
