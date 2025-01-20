/**
 * @file Logger.h
 * @brief Logger class definition
 * @author piotrek-pl
 * @date 2025-01-20 13:37:35
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
    void setLogLevel(LogLevel level);
    void setLogFile(const QString& filename);

    // Convenience methods
    void debug(const QString& message);
    void info(const QString& message);
    void warning(const QString& message);
    void error(const QString& message);
    void critical(const QString& message);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void rotateLogFile();
    QString levelToString(LogLevel level);

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
