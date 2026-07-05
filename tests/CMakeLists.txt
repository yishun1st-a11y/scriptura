cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Test Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test Core Network)

set(TEST_SOURCES
    main.cpp
    test_configvalidator.cpp
    ${CMAKE_SOURCE_DIR}/configvalidator.cpp
    test_pluginsystem.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(scriptura_tests
        ${TEST_SOURCES}
    )
else()
    add_executable(scriptura_tests
        ${TEST_SOURCES}
    )
endif()

target_link_libraries(scriptura_tests PRIVATE scriptura_core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)

# Include the parent directory for headers
target_include_directories(scriptura_tests PRIVATE ${CMAKE_SOURCE_DIR})

# Register the test with CTest
add_test(NAME scriptura_tests COMMAND scriptura_tests)
