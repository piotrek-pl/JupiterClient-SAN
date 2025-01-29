set(TEST_NAME ui_tests)

# Lista wszystkich potrzebnych plików źródłowych
set(TEST_SOURCES
    test_ui.cpp
    ${CMAKE_SOURCE_DIR}/src/network/Protocol.cpp
    ${CMAKE_SOURCE_DIR}/src/config/ConfigManager.cpp
    ${CMAKE_SOURCE_DIR}/src/network/NetworkManager.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/LoginWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/MainWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SearchDialog.cpp      # Dodano
    ${CMAKE_SOURCE_DIR}/src/ui/ChatWindow.cpp        # Dodano
    ${CMAKE_SOURCE_DIR}/src/ui/InvitationsDialog.cpp # Dodano
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp         # Dodano
)

# Zdefiniowanie plików .ui do przetworzenia przez UIC
set(UI_FILES
    ${CMAKE_SOURCE_DIR}/src/ui/LoginWindow.ui
    ${CMAKE_SOURCE_DIR}/src/ui/MainWindow.ui
    ${CMAKE_SOURCE_DIR}/src/ui/SearchDialog.ui       # Dodano
    ${CMAKE_SOURCE_DIR}/src/ui/ChatWindow.ui         # Dodano
    ${CMAKE_SOURCE_DIR}/src/ui/InvitationsDialog.ui  # Dodano
)

# Utworzenie listy plików nagłówkowych dla moc
set(MOC_HEADERS
    ${CMAKE_SOURCE_DIR}/src/ui/LoginWindow.h
    ${CMAKE_SOURCE_DIR}/src/ui/MainWindow.h
    ${CMAKE_SOURCE_DIR}/src/ui/SearchDialog.h        # Dodano
    ${CMAKE_SOURCE_DIR}/src/ui/ChatWindow.h          # Dodano
    ${CMAKE_SOURCE_DIR}/src/ui/InvitationsDialog.h   # Dodano
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.h           # Dodano
)

# Konfiguracja Qt
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network Test Core REQUIRED)

# Automatyczna obsługa moc, uic i rcc
qt_wrap_ui(UI_HEADERS ${UI_FILES})
qt_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

# Dodanie plików zasobów
set(RESOURCES_QRC "${CMAKE_SOURCE_DIR}/resources.qrc")

# Sprawdź czy plik istnieje
if(NOT EXISTS ${RESOURCES_QRC})
    message(FATAL_ERROR "Resource file not found: ${RESOURCES_QRC}")
endif()

qt_add_resources(QRC_SOURCES ${RESOURCES_QRC})

add_executable(${TEST_NAME}
    ${TEST_SOURCES}
    ${UI_HEADERS}
    ${MOC_SOURCES}
    ${QRC_SOURCES}
)

target_link_libraries(${TEST_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Widgets
)

target_include_directories(${TEST_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}
)

# Kopiowanie pliku konfiguracyjnego do katalogu testów
configure_file(
    ${CMAKE_SOURCE_DIR}/src/config/jupiter.conf
    ${CMAKE_CURRENT_BINARY_DIR}/config/jupiter.conf
    COPYONLY
)

# Utworzenie katalogu dla logów
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/logs)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
