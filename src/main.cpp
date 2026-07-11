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
#include <QFileInfo>

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

    // Parse command-line arguments:
    //   scriptura [--project <dir>] [file1 file2 ...]
    // A bare directory argument is treated as the project to open.
    QString initialProject;
    QStringList initialFiles;
    const QStringList cliArgs = a.arguments();
    for (int i = 1; i < cliArgs.size(); ++i) {
        const QString &arg = cliArgs.at(i);
        if (arg == "--project" && i + 1 < cliArgs.size()) {
            initialProject = cliArgs.at(++i);
        } else {
            QFileInfo fi(arg);
            if (fi.isDir()) {
                if (initialProject.isEmpty())
                    initialProject = arg;
            } else {
                initialFiles.append(arg);
            }
        }
    }

    a.setWindowIcon(QIcon(":/icon.png"));

    a.setStyleSheet(R"(
/* Modern Professional IDE Styling */

/* Base styling for all widgets */
QMainWindow, QWidget {
    background-color: palette(window);
    font-family: "DejaVu Sans Mono", "Consolas", "Courier New", monospace;
}

/* Group boxes with modern styling */
QGroupBox {
    border: 1px solid palette(mid);
    border-radius: 12px;
    margin-top: 12px;
    padding-top: 16px;
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 12px;
    padding: 0 8px;
    color: palette(text);
    font-weight: 600;
}

/* Tree views and similar widgets */
QTreeView,
QTabWidget::pane,
QPlainTextEdit,
QTextEdit,
QLineEdit,
QComboBox,
QAbstractItemView {
    border: 1px solid palette(mid);
    border-radius: 8px;
    background-color: palette(base);
}

/* Menu bars and toolbars */
QMenuBar,
QToolBar {
    background-color: palette(button);
    border-bottom: 1px solid palette(mid);
    padding: 2px;
}

QStatusBar {
    background-color: palette(button);
    border-top: 1px solid palette(mid);
    padding: 4px 8px;
    color: palette(text);
}

/* Menu styling */
QMenu {
    background-color: palette(button);
    border: 1px solid palette(mid);
    border-radius: 8px;
    padding: 4px;
}

/* Menu bar items */
QMenuBar::item {
    background: transparent;
    padding: 6px 12px;
    border-radius: 6px;
    color: palette(text);
}

QMenuBar::item:selected {
    background-color: palette(light);
    color: palette(highlight);
}

QMenuBar::item:pressed {
    background-color: palette(mid);
}

/* Menu items */
QMenu::item {
    background-color: transparent;
    padding: 6px 24px 6px 8px;
    border-radius: 4px;
    color: palette(text);
}

QMenu::item:selected {
    background-color: palette(highlight);
    color: palette(highlighted-text);
}

QMenu::item:disabled {
    color: palette(mid);
}

/* Toolbar items */
QToolBar::item {
    border-radius: 6px;
    padding: 2px 4px;
}

/* Modern tab styling */
QTabBar {
    background-color: palette(button);
    border: none;
    qproperty-drawBase: false;
}

QTabBar::tab {
    border: 1px solid palette(mid);
    border-bottom-left-radius: 0px;
    border-bottom-right-radius: 0px;
    border-top-left-radius: 8px;
    border-top-right-radius: 8px;
    background-color: palette(button);
    padding: 8px 16px;
    margin-right: 4px;
    color: palette(mid);
    min-width: 80px;
}

QTabBar::tab:selected {
    background-color: palette(base);
    border-bottom: 2px solid palette(highlight);
    color: palette(text);
    margin-bottom: -2px;
}

QTabBar::tab:hover:!selected {
    background-color: palette(light);
    color: palette(text);
}

QTabBar::tab:disabled {
    color: palette(mid);
}

/* Tab widget pane */
QTabWidget::pane {
    border: 1px solid palette(mid);
    border-top: none;
    border-radius: 0px 0px 8px 8px;
    background-color: palette(base);
}

/* Button styling */
QToolButton,
QPushButton,
QDialogButtonBox > QPushButton {
    border: 1px solid palette(mid);
    border-radius: 8px;
    padding: 8px 16px;
    background-color: palette(button);
    color: palette(text);
    min-height: 32px;
    min-width: 64px;
}

QToolButton:hover,
QPushButton:hover,
QDialogButtonBox > QPushButton:hover {
    background-color: palette(light);
    border-color: palette(text);
}

QToolButton:pressed,
QPushButton:pressed,
QDialogButtonBox > QPushButton:pressed {
    background-color: palette(mid);
    border-color: palette(dark);
}

QToolButton:disabled,
QPushButton:disabled,
QDialogButtonBox > QPushButton:disabled {
    background-color: palette(button);
    border-color: palette(mid);
    color: palette(mid);
}

QToolButton:checked {
    background-color: palette(highlight);
    border-color: palette(highlight);
    color: palette(highlighted-text);
}

/* Input field styling */
QLineEdit,
QComboBox {
    border: 1px solid palette(mid);
    border-radius: 6px;
    padding: 6px 10px;
    background-color: palette(base);
    color: palette(text);
    selection-background-color: palette(highlight);
    selection-color: palette(highlighted-text);
}

QLineEdit:focus,
QComboBox:focus {
    border: 2px solid palette(highlight);
    padding: 5px 9px;
    background-color: palette(base);
}

QLineEdit:disabled,
QComboBox:disabled {
    border-color: palette(mid);
    color: palette(mid);
    background-color: palette(button);
}

/* Text edit areas */
QPlainTextEdit,
QTextEdit {
    border: 1px solid palette(mid);
    border-radius: 6px;
    padding: 8px;
    background-color: palette(base);
    color: palette(text);
    selection-background-color: palette(highlight);
    selection-color: palette(highlighted-text);
}

QPlainTextEdit:focus,
QTextEdit:focus {
    border: 2px solid palette(highlight);
}

/* Tree view styling */
QTreeView {
    border: 1px solid palette(mid);
    border-radius: 6px;
    background-color: palette(base);
    padding: 4px;
}

QTreeView::item {
    padding: 4px 8px;
    border-radius: 4px;
    color: palette(text);
}

QTreeView::item:hover {
    background-color: palette(light);
}

QTreeView::item:selected {
    background-color: palette(highlight);
    color: palette(highlighted-text);
}

QTreeView::item:selected:!active {
    background-color: palette(mid);
}

/* Scrollbar styling */
QScrollBar:horizontal,
QScrollBar:vertical {
    background-color: palette(button);
    border-radius: 6px;
    margin: 0px;
}

QScrollBar::handle:horizontal,
QScrollBar::handle:vertical {
    background-color: palette(mid);
    border-radius: 6px;
    min-height: 20px;
    min-width: 20px;
}

QScrollBar::handle:horizontal:hover,
QScrollBar::handle:vertical:hover {
    background-color: palette(text);
}

QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal,
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0px;
    width: 0px;
    border: none;
    background: transparent;
}

/* Checkbox and radio button styling */
QCheckBox,
QRadioButton {
    spacing: 8px;
    color: palette(text);
}

QCheckBox::indicator,
QRadioButton::indicator {
    width: 16px;
    height: 16px;
    border-radius: 4px;
    border: 1px solid palette(mid);
    background-color: palette(base);
}

QRadioButton::indicator {
    border-radius: 8px;
}

QCheckBox::indicator:checked {
    background-color: palette(highlight);
    border-color: palette(highlight);
    image: url(:/icons/check.svg);
}

QRadioButton::indicator:checked {
    background-color: palette(highlight);
    border-color: palette(highlight);
    background-color: qradialgradient(cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.25 palette(highlight), stop:0.5 palette(base));
}

QCheckBox:disabled,
QRadioButton:disabled {
    color: palette(mid);
}

QCheckBox::indicator:disabled,
QRadioButton::indicator:disabled {
    border-color: palette(mid);
    background-color: palette(button);
}

/* Tooltip styling */
QToolTip {
    background-color: palette(tooltip-base);
    color: palette(tooltip-text);
    border: 1px solid palette(mid);
    border-radius: 6px;
    padding: 6px 10px;
    font-size: 12px;
}

/* Focus frame for keyboard navigation */
:focus {
    outline: none;
}

/* Status bar styling */
QStatusBar::item {
    border: none;
    padding: 0 8px;
}

/* Improved dialog buttons */
QDialogButtonBox {
    padding: 8px;
}

QDialogButtonBox > QPushButton {
    min-width: 80px;
    padding: 8px 16px;
}

/* Spin box styling */
QSpinBox {
    border: 1px solid palette(mid);
    border-radius: 6px;
    padding: 4px 8px;
    background-color: palette(base);
    color: palette(text);
}

QSpinBox:focus {
    border: 2px solid palette(highlight);
    padding: 3px 7px;
}

QSpinBox::up-button,
QSpinBox::down-button {
    border: none;
    background-color: palette(button);
    width: 16px;
}

QSpinBox::up-button:hover,
QSpinBox::down-button:hover {
    background-color: palette(light);
}

QSpinBox::up-button:pressed,
QSpinBox::down-button:pressed {
    background-color: palette(mid);
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
    QTimer::singleShot(4000, [splash, initialProject, initialFiles]() {
        // Close splash screen
        splash->close();
        splash->deleteLater();
        
        // Create and show main window
        MainWindow *mainWindow = new MainWindow(initialProject, initialFiles);
        mainWindow->show();
    });

    return a.exec();
}
