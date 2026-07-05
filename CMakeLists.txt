cmake_minimum_required(VERSION 3.16)

project(scriptura LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Network)

set(TS_FILES scriptura_en_001.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        codeeditor.cpp
        codeeditor.h
        crashhandler.h
        crashhandler.cpp
        lspclient.cpp
        lspclient.h
        problempanel.cpp
        problempanel.h
        todopanel.cpp
        todopanel.h
        gitpanel.cpp
        gitpanel.h
        terminalpanel.cpp
        terminalpanel.h
        splashscreen.h
        resources.qrc
        updater.cpp
        updater.h
        configvalidator.cpp
        configvalidator.h
)

set(PLUGIN_SYSTEM_SOURCES
        pluginfeature.h
        plugininterface.h
        plugincontext.h
        plugincontext.cpp
        eventbus.h
        eventbus.cpp
        servicelocator.h
        servicelocator.cpp
        dependencyresolver.h
        dependencyresolver.cpp
        pluginmanager.h
        pluginmanager.cpp
        pluginsettings.h
        pluginsettings.cpp
        pluginupdater.h
        pluginupdater.cpp
        plugincrashhandler.h
        plugincrashhandler.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(scriptura
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(scriptura SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(scriptura
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${PROJECT_SOURCES} ${TS_FILES})
endif()

add_library(scriptura_core STATIC ${PLUGIN_SYSTEM_SOURCES})
set_target_properties(scriptura_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    FOLDER "Internal"
)

target_link_libraries(scriptura_core PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(scriptura_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(scriptura PRIVATE scriptura_core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

if(WIN32)
    set_target_properties(scriptura PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

# Plugin system configuration
set(PLUGIN_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/scriptura/plugins")
set(PLUGIN_OUTPUT_DIR "${CMAKE_BINARY_DIR}/plugins")

set_target_properties(scriptura PROPERTIES
    QT_PLUGIN_PATH "${PLUGIN_OUTPUT_DIR}"
)

# Add plugin subdirectories
add_subdirectory(plugins/git)

# Add tests
add_subdirectory(tests)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set(BUNDLE_ID_OPTION "")
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.scriptura)
endif()
set_target_properties(scriptura PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS scriptura
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Desktop file for Linux
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/scriptura.desktop")
    install(FILES scriptura.desktop
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
    )
endif()

# Packaging support
set(CPACK_PACKAGE_NAME "scriptura")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple Qt-based text editor with project file browsing")
set(CPACK_PACKAGE_VENDOR "Scriptura")
set(CPACK_PACKAGE_CONTACT "maintainer@example.com")

# Linux packaging
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/snap/snapcraft.yaml")
    set(CPACK_GENERATOR "TGZ")
endif()

# macOS DMG packaging
if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "Scriptura")
    set(CPACK_DMG_FORMAT "UDBZ")
endif()

# Windows NSIS packaging
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
endif()

include(CPack)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(scriptura)
endif()

