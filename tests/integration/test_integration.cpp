#include <QtTest>
#include <QApplication>
#include "network/NetworkManager.h"
#include "network/Protocol.h"
#include "config/ConfigManager.h"
#include "utils/Logger.h"
#include <QSignalSpy>

class IntegrationTests : public QObject
{
    Q_OBJECT

private:
    NetworkManager& networkManager = NetworkManager::getInstance();
    QApplication* app = nullptr;

private slots:
    void initTestCase()
    {
        // Inicjalizacja QApplication jeśli nie istnieje
        if (!QApplication::instance()) {
            static int argc = 1;
            static char* argv[] = { (char*)"test" };
            app = new QApplication(argc, argv);
        }

        // Inicjalizacja loggera
        Logger::getInstance().setLogFile("logs/test.log");

        QVERIFY(&networkManager != nullptr);
    }

    void cleanupTestCase()
    {
        networkManager.disconnectFromServer();
        if (app) {
            delete app;
            app = nullptr;
        }
    }

    // Test integracji
    void testNetworkProtocolIntegration()
    {
        // Tworzymy wiadomość za pomocą namespace Protocol
        QJsonObject loginRequest = Protocol::MessageStructure::createLoginRequest("testuser", "password");

        // Sprawdzamy czy wiadomość jest dozwolona w obecnym stanie
        QVERIFY(Protocol::MessageValidation::isMessageAllowedInState(
            Protocol::MessageType::LOGIN,
            Protocol::SessionState::INITIAL
            ));

        // Test połączenia
        QSignalSpy connectSpy(&networkManager, SIGNAL(connected()));
        QVERIFY(connectSpy.isValid());

        // Próba połączenia
        networkManager.connectToServer();

        // Czekamy na sygnał connected (z timeoutem 5 sekund)
        QVERIFY(connectSpy.wait(5000));
    }

    void testMessageFlow()
    {
        // Sprawdzamy czy możemy wysyłać wiadomości
        QSignalSpy messageSpy(&networkManager, SIGNAL(messageReceived(QJsonObject)));
        QVERIFY(messageSpy.isValid());

        // Tworzymy i wysyłamy testową wiadomość
        QJsonObject chatMessage = Protocol::MessageStructure::createMessage(1, "Hello");
        networkManager.sendMessage(chatMessage);

        // Czekamy na odpowiedź (z timeoutem 5 sekund)
        QVERIFY(messageSpy.wait(5000));
    }

    void testAuthenticationFlow()
    {
        // Test procesu logowania
        QSignalSpy loginSpy(&networkManager, SIGNAL(loginSuccessful()));
        QVERIFY(loginSpy.isValid());

        // Próba logowania
        networkManager.login("testuser", "password");

        // Czekamy na sygnał sukcesu logowania
        QVERIFY(loginSpy.wait(5000));
        QVERIFY(networkManager.isAuthenticated());
    }
};

QTEST_MAIN(IntegrationTests)
#include "test_integration.moc"
