#include "customtitlebar.h"
#include "themeicons.h"
#include <QMouseEvent>
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

CustomTitleBar::CustomTitleBar(QWidget *parent)
    : QWidget(parent)
    , minimizeButton(nullptr)
    , maximizeButton(nullptr)
    , closeButton(nullptr)
    , titleLabel(nullptr)
    , menuButton(nullptr)
    , m_isDragging(false)
    , m_dragPosition(QPoint())
    , m_dropdownMenu(nullptr)
{
    setFixedHeight(32);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setupLayout();
    styleButtons();
}

void CustomTitleBar::setupLayout()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 4, 8, 4);
    layout->setSpacing(4);

    menuButton = new QPushButton(this);
    menuButton->setObjectName("TitleBarMenu");
    menuButton->setFixedSize(32, 24);
    menuButton->setToolTip(tr("Menu"));
    menuButton->setIcon(ThemeIcons::instance()->icon(":/icons/menu.svg"));
    menuButton->setIconSize(QSize(16, 16));
    connect(menuButton, &QPushButton::clicked, this, &CustomTitleBar::showMenu);
    layout->addWidget(menuButton);

    titleLabel = new QLabel(tr("Scriptura"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(10);
    titleFont.setWeight(QFont::DemiBold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: palette(text); background: transparent;");
    layout->addWidget(titleLabel, 1, Qt::AlignVCenter);

    minimizeButton = new QPushButton(this);
    maximizeButton = new QPushButton(this);
    closeButton = new QPushButton(this);

    minimizeButton->setObjectName("TitleBarMinimize");
    maximizeButton->setObjectName("TitleBarMaximize");
    closeButton->setObjectName("TitleBarClose");

    minimizeButton->setFixedSize(46, 24);
    maximizeButton->setFixedSize(46, 24);
    closeButton->setFixedSize(46, 24);

    layout->addWidget(minimizeButton);
    layout->addWidget(maximizeButton);
    layout->addWidget(closeButton);

    connect(minimizeButton, &QPushButton::clicked, this, [this]() { emit minimizeRequest(); });
    connect(maximizeButton, &QPushButton::clicked, this, [this]() { emit maximizeRequest(); });
    connect(closeButton, &QPushButton::clicked, this, [this]() { emit closeRequest(); });
}

void CustomTitleBar::styleButtons()
{
    const QString buttonStyle = R"(
        QPushButton {
            border: none;
            background-color: transparent;
            color: palette(text);
            border-radius: 0px;
            width: 46px;
            height: 24px;
            padding: 0px;
        }
        QPushButton:hover {
            background-color: rgba(128, 128, 128, 0.2);
        }
        QPushButton:pressed {
            background-color: rgba(128, 128, 128, 0.35);
        }
        QPushButton#TitleBarClose:hover {
            background-color: #e81123;
            color: white;
        }
        QPushButton#TitleBarClose:pressed {
            background-color: #bf0f1d;
            color: white;
        }
    )";

    minimizeButton->setStyleSheet(buttonStyle);
    maximizeButton->setStyleSheet(buttonStyle);
    closeButton->setStyleSheet(buttonStyle);
    menuButton->setStyleSheet(buttonStyle);
}

void CustomTitleBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QColor iconColor = palette().color(foregroundRole());

    // Minimize
    paintWindowControls(p, minimizeButton->geometry(), minimizeButton->underMouse(), minimizeButton->isDown(), QStringLiteral("\u2014"));
    // Maximize
    paintWindowControls(p, maximizeButton->geometry(), maximizeButton->underMouse(), maximizeButton->isDown(), QStringLiteral("\u25a1"));
    // Close
    paintWindowControls(p, closeButton->geometry(), closeButton->underMouse(), closeButton->isDown(), QStringLiteral("\u2715"));
}

void CustomTitleBar::paintWindowControls(QPainter &p, const QRect &buttonRect, bool hovered, bool pressed, const QString &glyph)
{
    if (glyph.isEmpty())
        return;

    QColor color = palette().color(foregroundRole());
    if (closeButton && buttonRect == closeButton->geometry()) {
        if (pressed) {
            color = Qt::white;
        } else if (hovered) {
            color = Qt::white;
        }
    }

    QFont font = p.font();
    font.setPixelSize(10);
    p.setFont(font);
    p.setPen(color);

    QRect textRect = buttonRect.adjusted(0, 2, 0, -2);
    p.drawText(textRect, Qt::AlignCenter, glyph);
}

void CustomTitleBar::handleMousePress(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPosition().toPoint();
        m_isDragging = true;
    }
}

void CustomTitleBar::handleMouseMove(QMouseEvent *event)
{
    if (m_isDragging) {
        QWidget *mainWindow = window();
        if (mainWindow) {
            QPoint delta = event->globalPosition().toPoint() - m_dragPosition;
            mainWindow->move(mainWindow->pos() + delta);
            m_dragPosition = event->globalPosition().toPoint();
        }
    }
}

void CustomTitleBar::stopDrag()
{
    m_isDragging = false;
}

void CustomTitleBar::showMenu()
{
    if (!m_dropdownMenu) {
        m_dropdownMenu = new QMenu(this);
        m_dropdownMenu->setObjectName("titleBarMenu");
        
        m_dropdownMenu->addAction(tr("&Open Project..."), this, []() {});
        m_dropdownMenu->addAction(tr("&Open File..."), this, []() {});
        m_dropdownMenu->addSeparator();
        m_dropdownMenu->addAction(tr("&Save"), this, []() {});
        m_dropdownMenu->addAction(tr("Save &As..."), this, []() {});
        m_dropdownMenu->addSeparator();
        m_dropdownMenu->addAction(tr("&Undo"), this, []() {});
        m_dropdownMenu->addAction(tr("&Redo"), this, []() {});
        m_dropdownMenu->addSeparator();
        m_dropdownMenu->addAction(tr("&Preferences..."), this, []() {});
    }
    
    QPoint menuPos = mapToGlobal(QPoint(0, height()));
    m_dropdownMenu->exec(menuPos);
}
