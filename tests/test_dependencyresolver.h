#ifndef TEST_DEPENDENCYRESOLVER_H
#define TEST_DEPENDENCYRESOLVER_H

#include <QObject>

class TestDependencyResolver : public QObject
{
    Q_OBJECT
private slots:
    void testBuildGraph();
    void testTopologicalSortSimple();
    void testTopologicalSortMultipleDeps();
    void testHasCircularDependency();
    void testNoCircularDependency();
    void testValidateMissingRequiredDependency();
    void testValidateOptionalDependencyMissing();
    void testValidateAllDependenciesPresent();
    void testTopologicalSortReturnsEmptyOnCircular();
};

#endif // TEST_DEPENDENCYRESOLVER_H
