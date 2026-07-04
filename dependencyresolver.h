#ifndef DEPENDENCYRESOLVER_H
#define DEPENDENCYRESOLVER_H

#include <QObject>
#include <QJsonObject>
#include <QHash>
#include <QStringList>
#include <QSet>

/**
 * @file dependencyresolver.h
 * @brief 定義依賴解析器，用於解析插件之間的依賴關係
 * 
 * DependencyResolver 負責：
 * - 驗證插件依賴的完整性
 * - 檢測循環依賴
 * - 計算正確的載入順序 (拓撲排序)
 */

/**
 * @class DependencyResolver
 * @brief 解析插件依賴關係
 * 
 * 使用拓撲排序演算法確保插件按正確的順序載入，
 * 並檢測是否存在循環依賴或缺失的依賴。
 */
class DependencyResolver
{
public:
    /**
     * @struct DependencyError
     * @brief 依賴錯誤資訊
     */
    struct DependencyError {
        QString pluginId;           ///< 插件 ID
        QString missingDependency;  ///< 缺失的依賴 ID
        bool isOptional;            ///< 是否為可選依賴
        
        DependencyError() : isOptional(false) {}
        DependencyError(const QString& p, const QString& d, bool opt)
            : pluginId(p), missingDependency(d), isOptional(opt) {}
    };

    /**
     * @brief 驗證插件依賴
     * @param plugins 插件元數據列表
     * @return 依賴錯誤列表
     */
    QList<DependencyError> validate(const QList<QJsonObject>& plugins);
    
    /**
     * @brief 計算拓撲排序結果
     * @param plugins 插件元數據列表
     * @return 按依賴順序排序的插件 ID 列表
     */
    QStringList topologicalSort(const QList<QJsonObject>& plugins);
    
    /**
     * @brief 檢查是否存在循環依賴
     * @param plugins 插件元數據列表
     * @return 存在循環依賴返回 true
     */
    bool hasCircularDependency(const QList<QJsonObject>& plugins);

private:
    /**
     * @brief 建立依賴圖
     * @param plugins 插件元數據列表
     * @return 插件 ID 到其依賴列表的映射
     */
    QHash<QString, QStringList> buildGraph(const QList<QJsonObject>& plugins);
    
    /**
     * @brief 深度優先搜尋拓撲排序
     * @param graph 依賴圖
     * @return 排序結果
     */
    QStringList dfsSort(const QHash<QString, QStringList>& graph);
    
    /**
     * @brief 檢測循環依賴 (DFS)
     * @param graph 依賴圖
     * @param node 當前節點
     * @param visited 已訪問節點集合
     * @param recStack 遞迴堆疊
     * @return 存在循環返回 true
     */
    bool detectCycle(const QHash<QString, QStringList>& graph,
                     const QString& node,
                     QSet<QString>& visited,
                     QSet<QString>& recStack);
};

#endif // DEPENDENCYRESOLVER_H