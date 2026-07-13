#include "windowanimator.h"
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QTimer>
#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QDebug>

WindowAnimator::WindowAnimator(QObject *parent)
    : QObject(parent)
    , m_slideAnimation(nullptr)
    , m_fadeAnimation(nullptr)
    , m_bounceTimer(nullptr)
    , m_bounceDirection(1)
    , m_isAnimating(false)
{
}

WindowAnimator::~WindowAnimator()
{
    stopAllAnimations();
    if (m_slideAnimation)
        delete m_slideAnimation;
    if (m_fadeAnimation)
        delete m_fadeAnimation;
    if (m_bounceTimer)
        delete m_bounceTimer;
}

void WindowAnimator::animatePanelSlide(QWidget *panel, bool show, int duration)
{
    if (!panel)
        return;

    // Clean up existing animation
    if (m_slideAnimation) {
        m_slideAnimation->stop();
        delete m_slideAnimation;
        m_slideAnimation = nullptr;
    }

    // Setup geometry animation
    m_slideAnimation = new QPropertyAnimation(panel, "geometry");
    m_slideAnimation->setDuration(duration);
    m_slideAnimation->setEasingCurve(QEasingCurve::OutCubic);

    QRect startGeometry = panel->geometry();
    QRect endGeometry = startGeometry;

    if (show) {
        // Slide in from bottom
        endGeometry.setY(panel->parentWidget()->height() - panel->height());
        m_slideAnimation->setStartValue(QRect(startGeometry.x(), panel->parentWidget()->height(), startGeometry.width(), startGeometry.height()));
        m_slideAnimation->setEndValue(endGeometry);
        panel->show();
    } else {
        // Slide out to bottom
        m_slideAnimation->setStartValue(startGeometry);
        m_slideAnimation->setEndValue(QRect(startGeometry.x(), panel->parentWidget()->height(), startGeometry.width(), startGeometry.height()));
        connect(m_slideAnimation, &QPropertyAnimation::finished, panel, &QWidget::hide);
    }

    m_isAnimating = true;
    m_slideAnimation->start();
    connect(m_slideAnimation, &QPropertyAnimation::finished, this, [this]() {
        m_isAnimating = false;
        if (m_slideAnimation) {
            m_slideAnimation->deleteLater();
            m_slideAnimation = nullptr;
        }
    });
}

void WindowAnimator::animatePanelFade(QWidget *panel, bool show, int duration)
{
    if (!panel)
        return;

    // Clean up existing animation
    if (m_fadeAnimation) {
        m_fadeAnimation->stop();
        delete m_fadeAnimation;
        m_fadeAnimation = nullptr;
    }

    // Setup opacity effect if not already present
    QGraphicsOpacityEffect *opacityEffect = qobject_cast<QGraphicsOpacityEffect*>(panel->graphicsEffect());
    if (!opacityEffect) {
        opacityEffect = new QGraphicsOpacityEffect(panel);
        panel->setGraphicsEffect(opacityEffect);
    }

    m_fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    m_fadeAnimation->setDuration(duration);
    m_fadeAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    if (show) {
        opacityEffect->setOpacity(0.0);
        panel->show();
        m_fadeAnimation->setStartValue(0.0);
        m_fadeAnimation->setEndValue(1.0);
    } else {
        opacityEffect->setOpacity(1.0);
        m_fadeAnimation->setStartValue(1.0);
        m_fadeAnimation->setEndValue(0.0);
        connect(m_fadeAnimation, &QPropertyAnimation::finished, panel, &QWidget::hide);
    }

    m_isAnimating = true;
    m_fadeAnimation->start();
    connect(m_fadeAnimation, &QPropertyAnimation::finished, this, [this]() {
        m_isAnimating = false;
        if (m_fadeAnimation) {
            m_fadeAnimation->deleteLater();
            m_fadeAnimation = nullptr;
        }
    });
}

void WindowAnimator::animateTitleBarBounce()
{
    if (!m_bounceTimer) {
        m_bounceTimer = new QTimer(this);
        m_bounceTimer->setInterval(16); // ~60fps
        connect(m_bounceTimer, &QTimer::timeout, this, [this]() {
            m_bounceDirection *= -1;
            m_bounceTimer->stop();
        });
    }
    m_bounceTimer->start();
}

void WindowAnimator::stopAllAnimations()
{
    if (m_slideAnimation) {
        m_slideAnimation->stop();
        m_slideAnimation->deleteLater();
        m_slideAnimation = nullptr;
    }
    if (m_fadeAnimation) {
        m_fadeAnimation->stop();
        m_fadeAnimation->deleteLater();
        m_fadeAnimation = nullptr;
    }
    if (m_bounceTimer) {
        m_bounceTimer->stop();
    }
    m_isAnimating = false;
}
