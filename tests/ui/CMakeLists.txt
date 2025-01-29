set(TEST_NAME ui_tests)

# Konfiguracja Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(TEST_SOURCES
    test_ui.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/LoginWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/MainWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ChatWindow.cpp          # Dodano
    ${CMAKE_SOURCE_DIR}/src/ui/SearchDialog.cpp        # Dodano
    ${CMAKE_SOURCE_DIR}/src/ui/InvitationsDialog.cpp   # Dodano
    ${CMAKE_SOURCE_DIR}/src/network/NetworkManager.cpp
    ${CMAKE_SOURCE_DIR}/src/network/Protocol.cpp
    ${CMAKE_SOURCE_DIR}/src/config/ConfigManager.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)

# Zdefiniowanie plików .ui do przetworzenia przez UIC
set(UI_FILES
    ${CMAKE_SOURCE_DIR}/src/ui/LoginWindow.ui
    ${CMAKE_SOURCE_DIR}/src/ui/MainWindow.ui
    ${CMAKE_SOURCE_DIR}/src/ui/ChatWindow.ui          # Dodano
    ${CMAKE_SOURCE_DIR}/src/ui/SearchDialog.ui        # Dodano
    ${CMAKE_SOURCE_DIR}/src/ui/InvitationsDialog.ui   # Dodano
)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test Widgets Network Core REQUIRED)

qt_wrap_ui(UI_HEADERS ${UI_FILES})

# Dodaj zasoby
set(RESOURCES_QRC "${CMAKE_SOURCE_DIR}/resources.qrc")
qt_add_resources(QRC_SOURCES ${RESOURCES_QRC})

add_executable(${TEST_NAME}
    ${TEST_SOURCES}
    ${UI_HEADERS}
    ${QRC_SOURCES}
)

target_link_libraries(${TEST_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Core
)

target_include_directories(${TEST_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
