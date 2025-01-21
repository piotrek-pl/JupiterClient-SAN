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

    // Tworzenie okna logowania
    LoginWindow* loginWindow = new LoginWindow();

    // Utworzenie wskaźnika na MainWindow (początkowo nullptr)
    MainWindow* mainWindow = nullptr;

    // Konfiguracja połączeń między oknami
    QObject::connect(loginWindow, &LoginWindow::loginSuccessful, [&mainWindow, loginWindow]() {
        // Tworzenie MainWindow dopiero po udanym logowaniu
        mainWindow = new MainWindow();

        // Połączenie sygnału zamknięcia MainWindow z zakończeniem aplikacji
        QObject::connect(mainWindow, &MainWindow::destroyed, []() {
            QApplication::quit();
        });

        mainWindow->show();
        loginWindow->hide();
        loginWindow->deleteLater();  // Bezpieczniejsze niż bezpośrednie delete
    });

    // Wyświetl okno logowania
    loginWindow->show();

    int result = a.exec();

    // Cleanup
    if (mainWindow) {
        delete mainWindow;
    }

    LOG_INFO("Application shutting down");
    return result;
}
