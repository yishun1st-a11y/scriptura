#ifndef SPLITMANAGER_H
#define SPLITMANAGER_H

#include <QObject>
#include <QPair>
#include <QList>
#include <QSplitter>

class CodeEditor;

class SplitManager : public QObject
{
    Q_OBJECT
public:
    explicit SplitManager(QObject *parent = nullptr);

    void splitHorizontal(CodeEditor *editor);
    void splitVertical(CodeEditor *editor);
    void closeSplit(CodeEditor *editor);
    void closeAllSplits();
    
    QList<QPair<CodeEditor*, QSplitter*>> splits() const { return m_splits; }
    CodeEditor* activeEditor() const { return m_activeEditor; }
    void setActiveEditor(CodeEditor *editor);

signals:
    void splitCreated(CodeEditor *editor, QSplitter *splitter);
    void splitClosed(CodeEditor *editor);
    void activeEditorChanged(CodeEditor *editor);

private:
    QList<QPair<CodeEditor*, QSplitter*>> m_splits;
    CodeEditor *m_activeEditor;
};

#endif // SPLITMANAGER_H
