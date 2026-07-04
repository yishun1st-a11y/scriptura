#include "mainwindow.h"
#include "crashhandler.h"
#include "splashscreen.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSettings>
#include <QPalette>
#include <QColor>
#include <QTimer>

// Theme helper function to get window color based on theme
QColor getThemeWindowColor(ThemeColorFamily family, ThemeMode mode) {
    bool isDark = (mode == ThemeMode::Dark);
    
    if (family == ThemeColorFamily::Default) {
        return isDark ? QColor(53, 53, 53) : QColor(255, 255, 255);
    }
    if (family == ThemeColorFamily::Blue) {
        return isDark ? QColor(25, 35, 50) : QColor(240, 248, 255);
    }
    if (family == ThemeColorFamily::Green) {
        return isDark ? QColor(25, 45, 30) : QColor(240, 255, 240);
    }
    if (family == ThemeColorFamily::Red) {
        return isDark ? QColor(45, 25, 25) : QColor(255, 245, 245);
    }
    if (family == ThemeColorFamily::Yellow) {
        return isDark ? QColor(45, 45, 25) : QColor(255, 255, 240);
    }
    if (family == ThemeColorFamily::Brown) {
        return isDark ? QColor(40, 30, 20) : QColor(255, 250, 240);
    }
    if (family == ThemeColorFamily::Cyan) {
        return isDark ? QColor(25, 45, 45) : QColor(240, 255, 255);
    }
    if (family == ThemeColorFamily::Violet) {
        return isDark ? QColor(35, 25, 50) : QColor(245, 240, 255);
    }
    return isDark ? QColor(53, 53, 53) : QColor(255, 255, 255);
}

// Theme helper function to extract family from legacy theme int
ThemeColorFamily getThemeFamily(int legacy) {
    if (legacy < 2) {
        return ThemeColorFamily::Default;
    }
    if (legacy >= 30) {
        return ThemeColorFamily::Default;
    }
    int familyIndex = (legacy - 2) / 4 + 1;
    return static_cast<ThemeColorFamily>(familyIndex);
}

// Theme helper function to extract mode from legacy theme int
ThemeMode getThemeMode(int legacy) {
    if (legacy < 2) {
        return static_cast<ThemeMode>(legacy);
    }
    if (legacy >= 30) {
        return static_cast<ThemeMode>(legacy - 30);
    }
    int remainder = (legacy - 2) % 4;
    return static_cast<ThemeMode>(remainder / 2);
}

int main(int argc, char *argv[])
{
    CrashHandler::install();

    QApplication::setOrganizationName("Scriptura");
    QApplication::setApplicationName("Scriptura");

    QApplication a(argc, argv);

    a.setStyleSheet(R"(
QMainWindow, QWidget {
    border-radius: 12px;
    background-color: palette(window);
}

QGroupBox {
    border: 1px solid palette(mid);
    border-radius: 12px;
    margin-top: 12px;
    padding-top: 12px;
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 12px;
    padding: 0 6px;
}

QTreeView,
QTabWidget::pane,
QPlainTextEdit,
QTextEdit,
QLineEdit,
QComboBox,
QAbstractItemView,
QMenuBar,
QToolBar,
QStatusBar,
QMenu {
    border: 1px solid palette(mid);
    border-radius: 12px;
    background-color: palette(base);
}

QMenuBar {
    padding: 4px;
}

QMenuBar::item,
QMenu::item,
QToolBar::item {
    border-radius: 8px;
    padding: 4px 8px;
}

QTabBar::tab {
    border: 1px solid palette(mid);
    border-bottom-left-radius: 0px;
    border-bottom-right-radius: 0px;
    border-top-left-radius: 12px;
    border-top-right-radius: 12px;
    background-color: palette(button);
    padding: 6px 12px;
    margin-right: 2px;
}

QTabBar::tab:selected {
    background-color: palette(base);
    border-bottom-color: palette(base);
}

QTabBar::tab:hover {
    background-color: palette(light);
}

QToolButton,
QPushButton,
QDialogButtonBox > QPushButton {
    border: 1px solid palette(mid);
    border-radius: 10px;
    padding: 6px 12px;
    background-color: palette(button);
}

QToolButton:hover,
QPushButton:hover,
QDialogButtonBox > QPushButton:hover {
    background-color: palette(light);
}

QLineEdit:focus,
QComboBox:focus,
QPlainTextEdit:focus,
QTextEdit:focus,
QTreeView:focus {
    border-color: palette(highlight);
}

QScrollBar:horizontal,
QScrollBar:vertical {
    border-radius: 6px;
    background: palette(alternate-base);
}

QScrollBar::handle:horizontal,
QScrollBar::handle:vertical {
    border-radius: 6px;
    background: palette(mid);
}

QScrollBar::add-line,
QScrollBar::sub-line {
    border-radius: 6px;
    background: transparent;
}
)");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "scriptura_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // Load theme before showing splash screen
    QSettings settings;
    int legacyTheme = settings.value("theme/selected", 0).toInt();
    ThemeColorFamily family = getThemeFamily(legacyTheme);
    ThemeMode mode = getThemeMode(legacyTheme);
    QColor themeWindowColor = getThemeWindowColor(family, mode);

    // Show splash screen with theme background
    SplashScreen *splash = new SplashScreen;
    splash->setThemeBackground(themeWindowColor);
    splash->showWithDelay(4000);  // 4 seconds

    // Timer to create and show main window after splash screen duration
    QTimer::singleShot(4000, [splash]() {
        // Close splash screen
        splash->close();
        splash->deleteLater();
        
        // Create and show main window
        MainWindow *mainWindow = new MainWindow;
        mainWindow->show();
    });

    return a.exec();
}
