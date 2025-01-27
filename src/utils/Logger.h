/**
 * @file Logger.h
 * @brief Logger class definition
 * @author piotrek-pl
 * @date 2025-01-27 09:13:35
 */

#pragma once

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <memory>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger : public QObject {
    Q_OBJECT

public:
    static Logger& getInstance();

    void log(LogLevel level, const QString& message);
    void setLogLevel(LogLevel level) { currentLevel = level; }
    void setLogFile(const QString& filename);

    // Convenience methods
    void debug(const QString& message) { log(LogLevel::DEBUG, message); }
    void info(const QString& message) { log(LogLevel::INFO, message); }
    void warning(const QString& message) { log(LogLevel::WARNING, message); }
    void error(const QString& message) { log(LogLevel::ERROR, message); }
    void critical(const QString& message) { log(LogLevel::CRITICAL, message); }

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // File management
    void rotateLogFile();
    void ensureDirectoryExists(const QString& filename);
    bool openLogFile(const QString& filename);
    void closeCurrentFile();

    // Log handling
    QString createLogEntry(LogLevel level, const QString& message);
    void writeLogEntry(const QString& entry);
    void checkFileSize();

    // File rotation
    void removeOldestLog(const QString& baseFilename);
    void rotateExistingLogs(const QString& baseFilename);
    void createNewLogFile(const QString& baseFilename);

    // Helpers
    QString levelToString(LogLevel level);
    LogLevel parseLogLevel(const QString& levelStr);
    void initializeFromConfig();

    std::unique_ptr<QFile> logFile;
    std::unique_ptr<QTextStream> logStream;
    LogLevel currentLevel;
    qint64 maxFileSize;
    int maxBackupCount;
};

// Makra dla łatwiejszego logowania
#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)
#define LOG_CRITICAL(msg) Logger::getInstance().critical(msg)
