cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Test Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test Core Network)

set(TEST_SOURCES
    main.cpp
    test_configvalidator.cpp
    ${CMAKE_SOURCE_DIR}/configvalidator.cpp
    # Plugin System Tests
    test_pluginsystem.cpp
    ${CMAKE_SOURCE_DIR}/dependencyresolver.cpp
    ${CMAKE_SOURCE_DIR}/eventbus.cpp
    ${CMAKE_SOURCE_DIR}/servicelocator.cpp
    ${CMAKE_SOURCE_DIR}/pluginsettings.cpp
    ${CMAKE_SOURCE_DIR}/permission.cpp
    ${CMAKE_SOURCE_DIR}/pluginmanager.cpp
    ${CMAKE_SOURCE_DIR}/plugincontext.cpp
    ${CMAKE_SOURCE_DIR}/pluginupdater.cpp
    ${CMAKE_SOURCE_DIR}/plugincrashhandler.cpp
    mainwindow_stub.cpp
)

# Plugin System Headers
set(PLUGIN_HEADERS
    ${CMAKE_SOURCE_DIR}/pluginfeature.h
    ${CMAKE_SOURCE_DIR}/plugininterface.h
    ${CMAKE_SOURCE_DIR}/plugincontext.h
    ${CMAKE_SOURCE_DIR}/eventbus.h
    ${CMAKE_SOURCE_DIR}/servicelocator.h
    ${CMAKE_SOURCE_DIR}/dependencyresolver.h
    ${CMAKE_SOURCE_DIR}/pluginmanager.h
    ${CMAKE_SOURCE_DIR}/pluginsettings.h
    ${CMAKE_SOURCE_DIR}/permission.h
    ${CMAKE_SOURCE_DIR}/pluginupdater.h
    ${CMAKE_SOURCE_DIR}/plugincrashhandler.h
    test_pluginsystem.h
    test_configvalidator.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(scriptura_tests
        ${TEST_SOURCES}
        ${PLUGIN_HEADERS}
    )
 else()
    add_executable(scriptura_tests
        ${TEST_SOURCES}
        ${PLUGIN_HEADERS}
    )
endif()

target_link_libraries(scriptura_tests PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)

# Include the parent directory for headers
target_include_directories(scriptura_tests PRIVATE ${CMAKE_SOURCE_DIR})

# Register the test with CTest
add_test(NAME scriptura_tests COMMAND scriptura_tests)
