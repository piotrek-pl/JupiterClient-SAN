/**
 * @file main.cpp
 * @brief Application entry point
 */

#include "ui/MainWindow.h"
#include "ui/LoginWindow.h"
#include "network/NetworkManager.h"
#include "utils/Logger.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Inicjalizacja loggera
    Logger::getInstance().setLogFile("jupiter_client.log");
    Logger::getInstance().setLogLevel(LogLevel::DEBUG);
    LOG_INFO("Application started");

    // Inicjalizacja NetworkManager
    auto& networkManager = NetworkManager::getInstance();

    // Połączenie z serwerem
    networkManager.connectToServer();

    // Tworzenie okien
    LoginWindow* loginWindow = new LoginWindow();
    MainWindow* mainWindow = new MainWindow();

    // Konfiguracja połączeń między oknami
    QObject::connect(loginWindow, &LoginWindow::loginSuccessful, [=]() {
        loginWindow->hide();
        mainWindow->show();
        delete loginWindow;  // LoginWindow nie będzie już potrzebne
    });

    // Wyświetl okno logowania
    loginWindow->show();

    int result = a.exec();
    LOG_INFO("Application shutting down");
    return result;
}
