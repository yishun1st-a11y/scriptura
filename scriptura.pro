TEMPLATE = app
TARGET = scriptura
QT += widgets network
INCLUDEPATH += .

HEADERS += codeeditor.h mainwindow.h crashhandler.h splashscreen.h \
           lspclient.h problempanel.h todopanel.h gitpanel.h \
           terminalpanel.h updater.h configvalidator.h
FORMS += mainwindow.ui
SOURCES += codeeditor.cpp main.cpp mainwindow.cpp crashhandler.cpp \
           lspclient.cpp problempanel.cpp todopanel.cpp gitpanel.cpp \
           terminalpanel.cpp updater.cpp configvalidator.cpp
TRANSLATIONS += scriptura_en_001.ts
