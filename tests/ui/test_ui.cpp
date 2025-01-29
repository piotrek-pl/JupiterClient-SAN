#include <QtTest>
#include <QApplication>
#include "ui/LoginWindow.h"
#include "ui/MainWindow.h"
#include <QLineEdit>
#include <QPushButton>
#include <QMenu>

class UITests : public QObject
{
    Q_OBJECT

private:
    std::unique_ptr<LoginWindow> loginWindow;
    std::unique_ptr<MainWindow> mainWindow;

private slots:
    void initTestCase()
    {
        loginWindow = std::make_unique<LoginWindow>();
        mainWindow = std::make_unique<MainWindow>();
    }

    void cleanupTestCase()
    {
        loginWindow.reset();
        mainWindow.reset();
    }

    // Test okna logowania
    void testLoginWindowUI()
    {
        // Używamy qobject_cast dla bezpiecznej konwersji
        QLineEdit* usernameField = loginWindow->findChild<QLineEdit*>("usernameInput");
        QLineEdit* passwordField = loginWindow->findChild<QLineEdit*>("passwordInput");
        QPushButton* loginButton = loginWindow->findChild<QPushButton*>("loginButton");

        QVERIFY(usernameField != nullptr);
        QVERIFY(passwordField != nullptr);
        QVERIFY(loginButton != nullptr);

        // Symulacja wprowadzania tekstu
        QTest::keyClicks(usernameField, "testuser");
        QTest::keyClicks(passwordField, "password");

        // Sprawdzanie sygnałów
        QSignalSpy loginSpy(loginWindow.get(), SIGNAL(loginRequested(QString,QString)));
        QVERIFY(loginSpy.isValid());

        // Symulacja kliknięcia z użyciem QTest
        QTest::mouseClick(loginButton, Qt::LeftButton);
        QCOMPARE(loginSpy.count(), 1);
    }

    // Test głównego okna
    void testMainWindowUI()
    {
        QVERIFY(mainWindow->isEnabled());

        // Test menu
        QMenu* fileMenu = mainWindow->findChild<QMenu*>("fileMenu");
        QVERIFY(fileMenu != nullptr);

        // Test widoczności elementów
        QWidget* chatArea = mainWindow->findChild<QWidget*>("chatArea");
        QVERIFY(chatArea != nullptr);
        QVERIFY(chatArea->isVisible());
    }
};

QTEST_MAIN(UITests)
#include "test_ui.moc"
