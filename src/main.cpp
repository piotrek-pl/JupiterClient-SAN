#include <QApplication>
#include "ui/MainWindow.h"
#include "ui/LoginWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    LoginWindow loginWindow;
    MainWindow mainWindow;

    QObject::connect(&loginWindow, &LoginWindow::loginSuccessful, [&]() {
        loginWindow.close();
        mainWindow.setSocket(loginWindow.getSocket());  // Przekazujemy gniazdo do MainWindow
        mainWindow.show();
    });

    loginWindow.show();

    return app.exec();
}
