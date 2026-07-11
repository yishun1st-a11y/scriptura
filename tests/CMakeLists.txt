find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Widgets Core Network)

set(TEST_SOURCES
    test_main.cpp
    test_workspace.h
    test_workspace.cpp
    test_configvalidator.h
    test_configvalidator.cpp
    test_httpclientpanel.h
    test_httpclientpanel.cpp
)

qt_add_executable(run_tests
    ${TEST_SOURCES}
)

set_target_properties(run_tests PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    FOLDER "Tests"
)

target_link_libraries(run_tests PRIVATE
    scriptura_core
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)

target_include_directories(run_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/internals
    ${CMAKE_SOURCE_DIR}/src/plugins
)

add_test(NAME scriptura_unit_tests COMMAND run_tests)
