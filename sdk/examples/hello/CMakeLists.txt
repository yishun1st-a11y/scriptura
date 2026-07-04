# Hello World Plugin Example
# CMakeLists.txt for building a simple Scriptura plugin

cmake_minimum_required(VERSION 3.16)
project(HelloPlugin LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Scriptura Plugin SDK
find_package(ScripturaPluginSDK REQUIRED COMPONENTS PluginInterface)

# Find Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Plugin library
add_library(helloplugin SHARED
    helloplugin.cpp
    helloplugin.h
)

target_link_libraries(helloplugin PRIVATE
    Qt6::Core
    Qt6::Widgets
    ScripturaPluginSDK::PluginInterface
)

target_include_directories(helloplugin PRIVATE
    ${CMAKE_SOURCE_DIR}
)

# Set output directory
set_target_properties(helloplugin PROPERTIES
    OUTPUT_NAME helloplugin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/hello
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/hello
)

# Install plugin
install(TARGETS helloplugin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/scriptura/plugins/hello
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/scriptura/plugins/hello
)

install(FILES plugin.json
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/scriptura/plugins/hello
)
