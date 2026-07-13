#ifndef WINDOWANIMATOR_H
#define WINDOWANIMATOR_H

#include <QObject>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>

class WindowAnimator : public QObject
{
    Q_OBJECT
public:
    explicit WindowAnimator(QObject *parent = nullptr);
    ~WindowAnimator();

    void animatePanelSlide(QWidget *panel, bool show, int duration = 200);
    void animatePanelFade(QWidget *panel, bool show, int duration = 150);
    void animateTitleBarBounce();
    void stopAllAnimations();

private:
    QPropertyAnimation *m_slideAnimation;
    QPropertyAnimation *m_fadeAnimation;
    QTimer *m_bounceTimer;
    int m_bounceDirection;
    bool m_isAnimating;
};

#endif // WINDOWANIMATOR_H
