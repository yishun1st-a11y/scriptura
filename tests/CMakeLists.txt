cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Test Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test Core Network)

set(TEST_SOURCES
    main.cpp
    test_configvalidator.cpp
    test_pluginsystem.cpp
)

# Manually moc only the test class headers. Disable AUTOMOC for this target
# so it does not re-generate moc for shared plugin/system headers that are
# already moc'd inside scriptura_core, which would cause duplicate symbols.
set(CMAKE_AUTOMOC OFF)

qt_wrap_cpp(TEST_MOC_SOURCES
    test_pluginsystem.h
    test_configvalidator.h
)

add_executable(scriptura_tests
    ${TEST_SOURCES}
    ${TEST_MOC_SOURCES}
)

target_link_libraries(scriptura_tests PRIVATE
    scriptura_core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)

target_include_directories(scriptura_tests PRIVATE ${CMAKE_SOURCE_DIR})

add_test(NAME scriptura_tests COMMAND scriptura_tests)
