set(TEST_NAME unit_tests)

# Lista źródeł potrzebnych do testów
set(TEST_SOURCES
    test_unit.cpp
    ${CMAKE_SOURCE_DIR}/src/network/Protocol.cpp
    ${CMAKE_SOURCE_DIR}/src/config/ConfigManager.cpp  # Dodano ConfigManager
)

add_executable(${TEST_NAME} ${TEST_SOURCES})

target_link_libraries(${TEST_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)

target_include_directories(${TEST_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Kopiowanie pliku konfiguracyjnego do katalogu testów
configure_file(
    ${CMAKE_SOURCE_DIR}/src/config/jupiter.conf
    ${CMAKE_CURRENT_BINARY_DIR}/config/jupiter.conf
    COPYONLY
)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
