#ifndef DEBUGGERGUTTER_H
#define DEBUGGERGUTTER_H

#include <QWidget>
#include <QSet>

class CodeEditor;

class DebuggerGutter : public QWidget
{
    Q_OBJECT
public:
    explicit DebuggerGutter(CodeEditor *editor, QWidget *parent = nullptr);

    void setBreakpoints(const QSet<int> &lines);
    QSet<int> breakpoints() const { return m_breakpoints; }
    void toggleBreakpoint(int line);
    void clearBreakpoints();

signals:
    void breakpointToggled(int line, bool enabled);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    CodeEditor *m_editor;
    QSet<int> m_breakpoints;
};

#endif // DEBUGGERGUTTER_H
