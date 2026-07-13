#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QPalette>
#include <QColor>
#include <QMap>
#include <QStringList>

class ThemeManager : public QObject
{
    Q_OBJECT
public:
    enum class ColorFamily {
        Default = 0,
        Blue = 1,
        Green = 2,
        Red = 3,
        Yellow = 4,
        Brown = 5,
        Cyan = 6,
        Violet = 7
    };

    enum class Mode {
        Light = 0,
        Dark = 1
    };

    enum class Feature {
        None = 0x0,
        HighContrast = 0x1
    };
    Q_DECLARE_FLAGS(Features, Feature)

    struct Theme {
        ColorFamily family;
        Mode mode;
        Features features;

        Theme(ColorFamily f = ColorFamily::Default, Mode m = Mode::Light, Features feat = Features())
            : family(f), mode(m), features(feat) {}

        bool isDark() const { return mode == Mode::Dark; }
        bool operator==(const Theme &other) const {
            return family == other.family && mode == other.mode && features == other.features;
        }
        bool operator!=(const Theme &other) const { return !(*this == other); }
    };

    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();

    // Theme application
    void applyTheme(const Theme &theme);
    Theme currentTheme() const { return m_currentTheme; }
    void setCurrentTheme(const Theme &theme);

    // Design tokens (for QSS generation)
    QString generateDesignTokens() const;
    QString generateGlobalStylesheet() const;

    // Palette generation
    QPalette buildBasePalette(ColorFamily family, Mode mode) const;
    QColor accentColor() const;
    QColor backgroundColor() const;
    QColor textColor() const;
    QColor borderColor() const;

signals:
    void themeChanged(const Theme &theme);

private:
    Theme m_currentTheme;
    QMap<ColorFamily, QMap<Mode, QColor>> m_backgroundCache;
    QMap<ColorFamily, QMap<Mode, QColor>> m_accentCache;

    void buildCaches();
    QColor familyColor(ColorFamily family, Mode mode, int alpha = 255) const;
    QStringList fontStack() const;
    QStringList monoFontStack() const;
};

#endif // THEMEMANAGER_H
