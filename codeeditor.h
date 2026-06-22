#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPainter>
#include <QPaintEvent>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QString>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QVector>
#include <QWidget>
#include <QColor>
#include <QEvent>

class CodeHighlighter : public QSyntaxHighlighter
{
public:
    explicit CodeHighlighter(QTextDocument *parent = nullptr);
    void setLanguage(const QString &language);
    void setDarkMode(bool dark);
    void setThemeColors(const QColor &keyword, const QColor &string, const QColor &comment,
                        const QColor &number, const QColor &preprocessor, const QColor &tag,
                        const QColor &attribute, const QColor &cssProperty,
                        const QColor &variable, const QColor &function, const QColor &escape,
                        const QColor &trailingSpace);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
        int captureIndex = 0;
    };

    void initializeFormats();
    void addRule(const QString &pattern, const QTextCharFormat &format, int captureIndex = 0);
    void setupCStyle();
    void setupPython();
    void setupJava();
    void setupJavaScript();
    void setupTypeScript();
    void setupRust();
    void setupGo();
    void setupShell();
    void setupHtml();
    void setupCss();
    void setupPlainText();

    void handleCStyleBlockComment(const QString &text);
    void handlePythonTripleString(const QString &text);
    void handleHtmlComment(const QString &text);

    QString language;
    QVector<HighlightingRule> rules;
    bool inBlockComment = false;
    bool inTripleString = false;
    bool inHtmlComment = false;
    QString tripleDelimiter;
    bool darkMode = false;

    QTextCharFormat keywordFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat preprocessorFormat;
    QTextCharFormat tagFormat;
    QTextCharFormat attributeFormat;
    QTextCharFormat cssPropertyFormat;
    QTextCharFormat variableFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat escapeFormat;
    QTextCharFormat trailingSpaceFormat;
};

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
    friend class LineNumberArea;
public:
    CodeEditor(QWidget *parent = nullptr);
    void setLanguageForFile(const QString &filePath);
    void setDarkMode(bool dark);
    void setThemeColors(const QColor &keyword, const QColor &string, const QColor &comment,
                        const QColor &number, const QColor &preprocessor, const QColor &tag,
                        const QColor &attribute, const QColor &cssProperty,
                        const QColor &variable, const QColor &function, const QColor &escape,
                        const QColor &trailingSpace = QColor());

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount = 0);
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();

private:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth() const;
    void drawIndentGuides(QPaintEvent *event);

    QWidget *lineNumberArea;
    CodeHighlighter *syntaxHighlighter;
    bool m_showIndentGuides = true;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor)
    {
        setObjectName("lineNumberArea");
    }

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

#endif // CODEEDITOR_H
