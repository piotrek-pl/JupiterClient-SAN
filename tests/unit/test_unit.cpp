#include <QtTest>
#include <QCoreApplication>
#include "network/Protocol.h"
#include "config/ConfigManager.h"
#include "utils/Logger.h"
#include <QSignalSpy>

class UnitTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qDebug() << "Starting unit tests";
        Logger::getInstance().setLogFile("logs/unit_test.log");
    }

    void cleanupTestCase()
    {
        qDebug() << "Finishing unit tests";
    }

    // Test struktury wiadomości protokołu
    void testProtocolMessageStructure()
    {
        // Test wiadomości logowania
        QJsonObject loginMsg = Protocol::MessageStructure::createLoginRequest("testuser", "password");
        QCOMPARE(loginMsg["type"].toString(), Protocol::MessageType::LOGIN);
        QCOMPARE(loginMsg["username"].toString(), "testuser");
        QVERIFY(loginMsg.contains("password"));
        QCOMPARE(loginMsg["protocol_version"].toInt(), 1);

        // Test wiadomości czatu
        QJsonObject chatMsg = Protocol::MessageStructure::createMessage(1, "Hello");
        QCOMPARE(chatMsg["type"].toString(), Protocol::MessageType::SEND_MESSAGE);
        QCOMPARE(chatMsg["content"].toString(), "Hello");
        QCOMPARE(chatMsg["receiver_id"].toInt(), 1);
        QVERIFY(chatMsg.contains("timestamp"));
    }

    // Test menedżera konfiguracji
    void testConfigManager()
    {
        ConfigManager& config = ConfigManager::getInstance();

        // Test konfiguracji połączenia
        auto connConfig = config.getConnectionConfig();
        QVERIFY(!connConfig.host.isEmpty());
        QVERIFY(connConfig.port > 0);
        QVERIFY(connConfig.maxReconnectAttempts > 0);

        // Test konfiguracji logowania
        auto logConfig = config.getLogConfig();
        QVERIFY(!logConfig.file.isEmpty());
        QVERIFY(logConfig.maxFileSize > 0);
        QVERIFY(logConfig.maxBackupCount > 0);
    }

    void testLogger()
    {
        Logger& logger = Logger::getInstance();

        // Test logowania różnych poziomów
        logger.debug("Test debug message");
        logger.info("Test info message");
        logger.warning("Test warning message");
        logger.error("Test error message");

        // Weryfikacja pliku logu
        QFile logFile("logs/unit_test.log");
        QVERIFY(logFile.exists());
        QVERIFY(logFile.open(QIODevice::ReadOnly | QIODevice::Text));
        QString logContent = QString::fromUtf8(logFile.readAll());

        QVERIFY(logContent.contains("Test debug message"));
        QVERIFY(logContent.contains("Test info message"));
        QVERIFY(logContent.contains("Test warning message"));
        QVERIFY(logContent.contains("Test error message"));
    }
};

QTEST_MAIN(UnitTests)
#include "test_unit.moc"
