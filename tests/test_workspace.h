#ifndef TEST_WORKSPACE_H
#define TEST_WORKSPACE_H

#include <QObject>

class TestWorkspace : public QObject
{
    Q_OBJECT
public slots:
    void testLoadValidWorkspace();
    void testLoadMissingFile();
    void testLoadInvalidJson();
    void testSaveAndReloadRoundTrip();
    void testAddRecentFileDedup();
    void testAddRecentFileCapacityCap();
    void testIsLoadedBeforeAndAfterSave();
};

#endif // TEST_WORKSPACE_H
