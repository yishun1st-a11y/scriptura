#include "splitmanager.h"
#include "codeeditor.h"

SplitManager::SplitManager(QObject *parent)
    : QObject(parent)
    , m_activeEditor(nullptr)
{
}

void SplitManager::splitHorizontal(CodeEditor *editor)
{
    if (!editor) return;
    
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(editor);
    
    CodeEditor *newEditor = new CodeEditor();
    newEditor->setDocument(editor->document());
    splitter->addWidget(newEditor);
    
    m_splits.append(qMakePair(editor, splitter));
    m_splits.append(qMakePair(newEditor, splitter));
    
    emit splitCreated(newEditor, splitter);
    setActiveEditor(newEditor);
}

void SplitManager::splitVertical(CodeEditor *editor)
{
    if (!editor) return;
    
    QSplitter *splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(editor);
    
    CodeEditor *newEditor = new CodeEditor();
    newEditor->setDocument(editor->document());
    splitter->addWidget(newEditor);
    
    m_splits.append(qMakePair(editor, splitter));
    m_splits.append(qMakePair(newEditor, splitter));
    
    emit splitCreated(newEditor, splitter);
    setActiveEditor(newEditor);
}

void SplitManager::closeSplit(CodeEditor *editor)
{
    if (!editor) return;
    
    for (int i = 0; i < m_splits.size(); ++i) {
        if (m_splits[i].first == editor) {
            QSplitter *splitter = m_splits[i].second;
            
            // Remove from list
            m_splits.removeAt(i);
            
            // If splitter has only one widget left, remove it
            if (splitter->count() == 1) {
                splitter->deleteLater();
            }
            
            emit splitClosed(editor);
            
            // Set new active editor
            if (!m_splits.isEmpty()) {
                setActiveEditor(m_splits.first().first);
            } else {
                setActiveEditor(nullptr);
            }
            
            return;
        }
    }
}

void SplitManager::closeAllSplits()
{
    while (!m_splits.isEmpty()) {
        closeSplit(m_splits.first().first);
    }
}

void SplitManager::setActiveEditor(CodeEditor *editor)
{
    if (m_activeEditor != editor) {
        m_activeEditor = editor;
        emit activeEditorChanged(editor);
    }
}
