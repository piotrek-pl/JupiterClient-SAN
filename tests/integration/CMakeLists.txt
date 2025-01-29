set(TEST_NAME integration_tests)

set(TEST_SOURCES
    test_integration.cpp
    ${CMAKE_SOURCE_DIR}/src/network/Protocol.cpp
    ${CMAKE_SOURCE_DIR}/src/config/ConfigManager.cpp
    ${CMAKE_SOURCE_DIR}/src/network/NetworkManager.cpp  # Dodano NetworkManager
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp  # Dodano Logger jeśli istnieje
)

add_executable(${TEST_NAME} ${TEST_SOURCES})

target_link_libraries(${TEST_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Widgets  # Dodano dla QApplication
)

target_include_directories(${TEST_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Kopiowanie plików konfiguracyjnych
configure_file(
    ${CMAKE_SOURCE_DIR}/src/config/jupiter.conf
    ${CMAKE_CURRENT_BINARY_DIR}/config/jupiter.conf
    COPYONLY
)

# Utwórz katalog dla logów
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/logs)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
