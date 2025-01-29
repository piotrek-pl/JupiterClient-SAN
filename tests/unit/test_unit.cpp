#include <QtTest>
#include "network/Protocol.h"
#include "config/ConfigManager.h"
#include <QCoreApplication>

class UnitTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qDebug() << "Starting unit tests";
    }

    void cleanupTestCase()
    {
        qDebug() << "Finishing unit tests";
    }

    // Test protokołu
    void testProtocolMessageStructure()
    {
        // Używamy namespace Protocol i jego statycznych metod
        QJsonObject loginRequest = Protocol::MessageStructure::createLoginRequest("test", "password");
        QVERIFY(!loginRequest.isEmpty());
        QCOMPARE(loginRequest["type"].toString(), Protocol::MessageType::LOGIN);
    }

    // Test ConfigManager
    void testConfigManager()
    {
        // Używamy singletona zamiast tworzenia instancji
        ConfigManager& config = ConfigManager::getInstance();
        auto connectionConfig = config.getConnectionConfig();
        QVERIFY(!connectionConfig.host.isEmpty());
        QVERIFY(connectionConfig.port > 0);
    }
};

QTEST_MAIN(UnitTests)
#include "test_unit.moc"
