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
        if (!QApplication::instance()) {
            static int argc = 1;
            static char* argv[] = { (char*)"test" };
            app = new QApplication(argc, argv);
        }

        Logger::getInstance().setLogFile("logs/integration_test.log");

        // Sprawdź czy serwer jest dostępny
        QTcpSocket socket;
        socket.connectToHost("127.0.0.1", 1234);
        QVERIFY2(socket.waitForConnected(1000), "Server is not running!");
        socket.disconnectFromHost();
    }

    void cleanupTestCase()
    {
        networkManager.disconnectFromServer();
        if (app) {
            delete app;
            app = nullptr;
        }
    }

    // Test 1: Połączenie z serwerem
    void testNetworkProtocolIntegration()
    {
        QSignalSpy connectSpy(&networkManager, SIGNAL(connected()));
        QVERIFY(connectSpy.isValid());

        networkManager.connectToServer();
        QVERIFY2(connectSpy.wait(5000), "Failed to connect to server");
        QVERIFY(networkManager.isConnected());
    }

    // Test 2: Logowanie
    void testAuthenticationFlow()
    {
        QVERIFY2(networkManager.isConnected(), "Not connected to server");

        QSignalSpy loginSpy(&networkManager, SIGNAL(loginSuccessful()));
        QVERIFY(loginSpy.isValid());

        networkManager.login("test1", "test1");  // Używamy prawidłowych danych logowania
        QVERIFY2(loginSpy.wait(5000), "Login failed");
        QVERIFY(networkManager.isAuthenticated());
    }

    // Test 3: Wysyłanie wiadomości (po zalogowaniu)
    void testMessageFlow()
    {
        QVERIFY2(networkManager.isConnected(), "Not connected to server");
        QVERIFY2(networkManager.isAuthenticated(), "Not authenticated");

        QSignalSpy messageSpy(&networkManager, SIGNAL(messageReceived(QJsonObject)));
        QVERIFY(messageSpy.isValid());

        // Czekamy krótko, aby upewnić się, że poprzednie operacje się zakończyły
        QTest::qWait(500);

        QJsonObject chatMessage = Protocol::MessageStructure::createMessage(1, "Hello");
        networkManager.sendMessage(chatMessage);
        QVERIFY2(messageSpy.wait(5000), "No response received from server");

        // Sprawdź odpowiedź serwera
        if (!messageSpy.isEmpty()) {
            QList<QVariant> arguments = messageSpy.takeFirst();
            QJsonObject response = arguments.at(0).toJsonObject();
            QVERIFY(response.contains("type"));
            // Możemy dodać więcej weryfikacji odpowiedzi
        }
    }
};

QTEST_MAIN(IntegrationTests)
#include "test_integration.moc"
