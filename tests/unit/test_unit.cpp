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
        QString testFile = "logs/unit_test.log";

        // Ustaw plik logów
        logger.setLogFile(testFile);

        // Małe opóźnienie na utworzenie pliku
        QTest::qWait(100);

        // Test logowania
        QString infoMsg = "Test info message";
        QString warningMsg = "Test warning message";
        QString errorMsg = "Test error message";

        logger.info(infoMsg);
        logger.warning(warningMsg);
        logger.error(errorMsg);

        // Małe opóźnienie na zapisanie wiadomości
        QTest::qWait(100);

        // Weryfikacja pliku logu
        QFile logFile(testFile);
        QVERIFY2(logFile.exists(), "Log file does not exist");
        QVERIFY2(logFile.open(QIODevice::ReadOnly | QIODevice::Text), "Cannot open log file");
        QString logContent = QString::fromUtf8(logFile.readAll());
        logFile.close();

        // Debug output w przypadku błędu
        qDebug() << "Log file content:";
        qDebug().noquote() << logContent;

        // Sprawdź czy wiadomości zostały zapisane
        QVERIFY2(logContent.contains(infoMsg), "Info message not found in log");
        QVERIFY2(logContent.contains(warningMsg), "Warning message not found in log");
        QVERIFY2(logContent.contains(errorMsg), "Error message not found in log");

        // Sprawdź format timestampa używając QRegularExpression
        QRegularExpression timestampRegex("\\[\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3}\\]");
        QVERIFY2(timestampRegex.match(logContent).hasMatch(), "Invalid timestamp format in log");
    }
};

QTEST_MAIN(UnitTests)
#include "test_unit.moc"
