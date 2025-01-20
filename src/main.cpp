/**
 * @file main.cpp
 * @brief Main application entry point
 * @author piotrek-pl
 * @date 2025-01-20 13:46:30
 */

#include <QApplication>
#include <QDir>
#include "ui/MainWindow.h"
#include "ui/LoginWindow.h"
#include "config/ConfigManager.h"
#include "utils/Logger.h"
#include "network/Protocol.h"

void initializeApplication() {
    // Tworzenie katalogu konfiguracyjnego jeśli nie istnieje
    QString configDir = QApplication::applicationDirPath() + "/config";
    QString logsDir = QApplication::applicationDirPath() + "/logs";

    if (!QDir(configDir).exists()) {
        QDir().mkpath(configDir);
    }
    if (!QDir(logsDir).exists()) {
        QDir().mkpath(logsDir);
    }

    // Inicjalizacja loggera i konfiguracji
    try {
        auto& config = ConfigManager::getInstance();
        auto& logger = Logger::getInstance();

        LOG_INFO("Jupiter Client starting...");
        LOG_INFO(QString("Protocol version: %1").arg(Protocol::PROTOCOL_VERSION));
        LOG_INFO(QString("Configuration loaded from: %1/jupiter.conf")
                     .arg(configDir));

        auto connectionConfig = config.getConnectionConfig();
        LOG_INFO(QString("Server configuration - Host: %1, Port: %2")
                     .arg(connectionConfig.host)
                     .arg(connectionConfig.port));
    }
    catch (const std::exception& e) {
        qCritical() << "Failed to initialize application:" << e.what();
        throw;
    }
}

void cleanupApplication() {
    LOG_INFO("Application shutdown initiated");

    try {
        // Tutaj możesz dodać dodatkowe operacje czyszczenia
        LOG_INFO("Cleanup completed successfully");
    }
    catch (const std::exception& e) {
        LOG_ERROR(QString("Cleanup failed: %1").arg(e.what()));
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Ustawienie informacji o aplikacji
    QApplication::setApplicationName("Jupiter Client");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("JupiterTeam");
    QApplication::setOrganizationDomain("jupiter.pl");

    try {
        initializeApplication();

        // Utworzenie głównych okien aplikacji
        auto loginWindow = new LoginWindow();
        auto mainWindow = new MainWindow();

        // Połączenie sygnałów między oknami
        QObject::connect(loginWindow, &LoginWindow::loginSuccessful, [=]() {
            LOG_INFO("Login successful, switching to main window");
            loginWindow->hide();
            mainWindow->setSocket(loginWindow->getSocket());
            mainWindow->show();
        });

        QObject::connect(loginWindow, &LoginWindow::registrationSuccessful, [=]() {
            LOG_INFO("Registration successful");
            // Możesz dodać dodatkową logikę po udanej rejestracji
        });

        // Obsługa zamykania aplikacji
        QObject::connect(&app, &QApplication::aboutToQuit, [=]() {
            LOG_INFO("Application shutdown requested");
            delete loginWindow;
            delete mainWindow;
            cleanupApplication();
        });

        // Uruchomienie aplikacji
        loginWindow->show();
        LOG_INFO("Login window displayed");

        return app.exec();
    }
    catch (const std::exception& e) {
        LOG_CRITICAL(QString("Critical error occurred: %1").arg(e.what()));
        return 1;
    }
}
