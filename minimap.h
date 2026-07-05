#ifndef MINIMAP_H
#define MINIMAP_H

#include <QWidget>
#include <QPlainTextEdit>

class Minimap : public QWidget
{
    Q_OBJECT
public:
    explicit Minimap(QPlainTextEdit *editor, QWidget *parent = nullptr);

    void setDocument(QTextDocument *document);
    void setVisible(bool visible) override;

signals:
    void viewportRequested(int position);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateGeometry();
    void onDocumentChanged();

private:
    QPlainTextEdit *m_editor;
    QTextDocument *m_document;
    QRect m_visibleRegion;
    
    static constexpr int SCALE_FACTOR = 10;
};

#endif // MINIMAP_H
