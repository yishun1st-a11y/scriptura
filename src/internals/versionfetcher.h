#ifndef VERSIONFETCHER_H
#define VERSIONFETCHER_H

#include <QObject>
#include <QString>

/**
 * @file versionfetcher.h
 * @brief 取得 Scriptura Core 版本資訊
 * 
 * 版本號在建置時從 GitHub Release tag 寫入，
 * 執行時直接讀取，不依賴網路請求。
 */

class VersionFetcher : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 取得當前 Core 版本
     * @return 版本字串 (如 "0.2.0")
     */
    static QString coreVersion();
};

#endif // VERSIONFETCHER_H
