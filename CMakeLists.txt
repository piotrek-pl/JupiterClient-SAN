cmake_minimum_required(VERSION 3.16)

project(JupiterClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
    src/main.cpp
    src/ui/LoginWindow.cpp
    src/ui/MainWindow.cpp
    src/ui/ChatWindow.cpp
    src/network/Client.cpp
    src/network/Protocol.cpp
    src/models/User.cpp
    src/config/ConfigManager.cpp
    src/utils/Logger.cpp
    src/ui/LoginWindow.h
    src/ui/MainWindow.h
    src/ui/ChatWindow.h
    src/network/Client.h
    src/network/Protocol.h
    src/models/User.h
    src/config/ConfigManager.h
    src/utils/Logger.h
    src/ui/LoginWindow.ui
    src/ui/MainWindow.ui
    src/ui/ChatWindow.ui
)

# Konfiguracja pliku konfiguracyjnego
configure_file(${CMAKE_SOURCE_DIR}/src/config/jupiter.conf
               ${CMAKE_BINARY_DIR}/config/jupiter.conf
               COPYONLY)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(JupiterClient
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(JupiterClient SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(JupiterClient
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(JupiterClient PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# Dodaj ścieżki include
target_include_directories(JupiterClient PRIVATE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# Instalacja
include(GNUInstallDirs)
install(TARGETS JupiterClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Instalacja pliku konfiguracyjnego
install(FILES ${CMAKE_SOURCE_DIR}/src/config/jupiter.conf
        DESTINATION ${CMAKE_INSTALL_PREFIX}/config)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(JupiterClient)
endif()
