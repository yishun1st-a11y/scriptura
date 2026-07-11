#ifndef TODOPANEL_H
#define TODOPANEL_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>

class TodoPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TodoPanel(QWidget *parent = nullptr);
    ~TodoPanel();

    void parseDocument(const QString &text, const QString &fileName = QString());

signals:
    void todoActivated(int line);

private slots:
    void onItemActivated(QListWidgetItem *item);

private:
    QListWidget *m_list;
    QVBoxLayout *m_mainLayout;
};

#endif // TODOPANEL_H
