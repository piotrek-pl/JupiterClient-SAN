/**
 * @file test_ui.cpp
 * @brief Testy UI Jupiter Client
 * @author piotrek-pl
 * @date 2025-01-29 19:13:13
 */

#include <QtTest>
#include <QApplication>
#include "ui/LoginWindow.h"
#include "ui/MainWindow.h"
#include <QLineEdit>
#include <QPushButton>
#include <QMenu>
#include <QMenuBar>
#include <QComboBox>

class UITests : public QObject
{
    Q_OBJECT

private:
    std::unique_ptr<LoginWindow> loginWindow;
    std::unique_ptr<MainWindow> mainWindow;

private slots:
    void initTestCase()
    {
        // Inicjalizacja okien
        loginWindow = std::make_unique<LoginWindow>();
        mainWindow = std::make_unique<MainWindow>();

        loginWindow->show();
        mainWindow->show();
        QTest::qWait(250);
    }

    void cleanupTestCase()
    {
        // Czyszczenie
        if (loginWindow) loginWindow->hide();
        if (mainWindow) mainWindow->hide();

        loginWindow.reset();
        mainWindow.reset();
    }

    void testLoginWindowUI()
    {
        // 1. Najpierw dodajmy więcej debugowania
        qDebug() << "\nInitial state:";
        qDebug() << "NetworkManager connected:" << NetworkManager::getInstance().isConnected();

        // 2. Przygotowanie okna do testów
        auto* networkManager = &NetworkManager::getInstance();

        // 3. Pobierz elementy UI przed emisją sygnałów
        QLineEdit* usernameField = loginWindow->findChild<QLineEdit*>("usernameLineEdit");
        QLineEdit* passwordField = loginWindow->findChild<QLineEdit*>("passwordLineEdit");
        QPushButton* loginButton = loginWindow->findChild<QPushButton*>("loginButton");

        QVERIFY2(usernameField != nullptr, "Username field not found");
        QVERIFY2(passwordField != nullptr, "Password field not found");
        QVERIFY2(loginButton != nullptr, "Login button not found");

        // 4. Emulacja połączenia
        networkManager->connectToServer();

        // Czekamy na połączenie
        QTest::qWait(1000);

        // Emitujemy sygnały w odpowiedniej kolejności
        emit networkManager->connectionStatusChanged("Connected to server");
        emit networkManager->connected();

        // Dajemy czas na przetworzenie sygnałów
        QApplication::processEvents();
        QTest::qWait(500);

        qDebug() << "\nAfter connection:";
        qDebug() << "NetworkManager connected:" << networkManager->isConnected();
        qDebug() << "Login button enabled:" << loginButton->isEnabled();

        // 5. Ustawienie tekstu w polach
        usernameField->setText("testuser");
        QApplication::processEvents();
        QTest::qWait(250);

        passwordField->setText("password");
        QApplication::processEvents();
        QTest::qWait(250);

        qDebug() << "\nAfter setting text:";
        qDebug() << "Username:" << usernameField->text();
        qDebug() << "Password:" << passwordField->text();
        qDebug() << "Login button enabled:" << loginButton->isEnabled();

        // 6. Wymuszenie walidacji
        emit usernameField->textChanged(usernameField->text());
        emit passwordField->textChanged(passwordField->text());
        QApplication::processEvents();
        QTest::qWait(250);

        // 7. Sprawdzenie stanu końcowego
        qDebug() << "\nFinal state:";
        qDebug() << "Username:" << usernameField->text();
        qDebug() << "Password:" << passwordField->text();
        qDebug() << "NetworkManager connected:" << networkManager->isConnected();
        qDebug() << "Login button enabled:" << loginButton->isEnabled();

        // 8. Testy właściwe
        QCOMPARE(usernameField->text(), QString("testuser"));
        QCOMPARE(passwordField->text(), QString("password"));
        QVERIFY2(loginButton->isEnabled(), "Login button is not enabled");

        // 9. Kliknięcie przycisku
        if(loginButton->isEnabled()) {
            loginButton->click();
        }
        QTest::qWait(500);
    }

    void testRegistrationMode()
    {
        // Pobranie elementów UI
        QLineEdit* emailField = loginWindow->findChild<QLineEdit*>("emailLineEdit");
        QLineEdit* usernameField = loginWindow->findChild<QLineEdit*>("usernameLineEdit");
        QLineEdit* passwordField = loginWindow->findChild<QLineEdit*>("passwordLineEdit");
        QPushButton* registerButton = loginWindow->findChild<QPushButton*>("registerButton");
        QPushButton* backToLoginButton = loginWindow->findChild<QPushButton*>("backToLoginButton");
        QPushButton* loginButton = loginWindow->findChild<QPushButton*>("loginButton");

        QVERIFY2(emailField != nullptr, "Email field not found");
        QVERIFY2(registerButton != nullptr, "Register button not found");
        QVERIFY2(backToLoginButton != nullptr, "Back to login button not found");

        // Stan początkowy
        qDebug() << "\nInitial state before any action:";
        qDebug() << "Email field visible:" << emailField->isVisible();
        qDebug() << "Register button visible:" << registerButton->isVisible();
        qDebug() << "Back button visible:" << backToLoginButton->isVisible();
        qDebug() << "Login button visible:" << loginButton->isVisible();
        qDebug() << "Register button enabled:" << registerButton->isEnabled();

        // Bezpośrednie wywołanie click() na przycisku rejestracji
        registerButton->click();
        QApplication::processEvents();
        QTest::qWait(500);

        qDebug() << "\nAfter register button click:";
        qDebug() << "Email field visible:" << emailField->isVisible();
        qDebug() << "Register button visible:" << registerButton->isVisible();
        qDebug() << "Back button visible:" << backToLoginButton->isVisible();
        qDebug() << "Login button visible:" << loginButton->isVisible();

        // Wprowadzamy dane rejestracji
        if(emailField->isVisible()) {
            emailField->setFocus();
            QTest::keyClicks(emailField, "test@example.com");
            QApplication::processEvents();
            QTest::qWait(250);

            usernameField->setFocus();
            QTest::keyClicks(usernameField, "testuser");
            QApplication::processEvents();
            QTest::qWait(250);

            passwordField->setFocus();
            QTest::keyClicks(passwordField, "password");
            QApplication::processEvents();
            QTest::qWait(250);
        }

        // Weryfikacja stanu końcowego
        qDebug() << "\nFinal state:";
        qDebug() << "Email field text:" << emailField->text();
        qDebug() << "Username field text:" << usernameField->text();
        qDebug() << "Password field text:" << passwordField->text();
        qDebug() << "Email field visible:" << emailField->isVisible();
        qDebug() << "Register button visible:" << registerButton->isVisible();
        qDebug() << "Back button visible:" << backToLoginButton->isVisible();
        qDebug() << "Login button visible:" << loginButton->isVisible();
    }

    void testMainWindowUI()
    {
        // Debug menu
        qDebug() << "Available menus in MainWindow:";
        const QList<QMenu*> menus = mainWindow->findChildren<QMenu*>();
        for(const auto& menu : menus) {
            qDebug() << "Menu:" << menu->objectName() << menu->title()
            << "Visible:" << menu->isVisible();
        }

        // Test głównego okna
        QVERIFY2(mainWindow->isVisible(), "Main window is not visible");

        // Test menu
        QMenu* fileMenu = mainWindow->findChild<QMenu*>("menuFile");
        QVERIFY2(fileMenu != nullptr, "File menu not found");

        QMenuBar* menuBar = mainWindow->findChild<QMenuBar*>("menubar");
        QVERIFY2(menuBar != nullptr, "Menu bar not found");

        // Test widgetów
        QWidget* centralWidget = mainWindow->findChild<QWidget*>("centralwidget");
        QVERIFY2(centralWidget != nullptr, "Central widget not found");
        QVERIFY2(centralWidget->isVisible(), "Central widget is not visible");

        // Test listy znajomych
        QListWidget* friendsList = mainWindow->findChild<QListWidget*>("friendsList");
        QVERIFY2(friendsList != nullptr, "Friends list not found");
        QVERIFY2(friendsList->isVisible(), "Friends list is not visible");

        // Test combo box statusu
        QComboBox* statusComboBox = mainWindow->findChild<QComboBox*>("statusComboBox");
        QVERIFY2(statusComboBox != nullptr, "Status combo box not found");
        QVERIFY2(statusComboBox->isVisible(), "Status combo box is not visible");

        // Test akcji menu
        QAction* searchAction = mainWindow->findChild<QAction*>("actionSearch");
        QVERIFY2(searchAction != nullptr, "Search action not found");

        QAction* exitAction = mainWindow->findChild<QAction*>("actionExit");
        QVERIFY2(exitAction != nullptr, "Exit action not found");

        QAction* aboutAction = mainWindow->findChild<QAction*>("actionAbout");
        QVERIFY2(aboutAction != nullptr, "About action not found");
    }
};

QTEST_MAIN(UITests)
#include "test_ui.moc"
