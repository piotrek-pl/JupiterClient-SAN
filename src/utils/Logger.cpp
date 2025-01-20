/**
 * @file Logger.cpp
 * @brief Logger class implementation
 * @author piotrek-pl
 * @date 2025-01-20 13:37:35
 */

#include "Logger.h"
#include "config/ConfigManager.h"
#include <QDateTime>
#include <QDir>

Logger::Logger() : currentLevel(LogLevel::INFO) {
    const auto& config = ConfigManager::getInstance();
    auto logConfig = config.getLogConfig();

    setLogFile(logConfig.file);
    maxFileSize = logConfig.maxFileSize;
    maxBackupCount = logConfig.maxBackupCount;

    QString logLevelStr = logConfig.level;
    if (logLevelStr == "DEBUG") currentLevel = LogLevel::DEBUG;
    else if (logLevelStr == "INFO") currentLevel = LogLevel::INFO;
    else if (logLevelStr == "WARNING") currentLevel = LogLevel::WARNING;
    else if (logLevelStr == "ERROR") currentLevel = LogLevel::ERROR;
    else if (logLevelStr == "CRITICAL") currentLevel = LogLevel::CRITICAL;
}

Logger::~Logger() {
    if (logStream) logStream->flush();
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogFile(const QString& filename) {
    QString dirPath = QFileInfo(filename).absolutePath();
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    logFile = std::make_unique<QFile>(filename);
    if (!logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open log file:" << filename;
        return;
    }
    logStream = std::make_unique<QTextStream>(logFile.get());
}

void Logger::log(LogLevel level, const QString& message) {
    if (level < currentLevel) return;

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString logEntry = QString("[%1] [%2] %3\n")
                           .arg(timestamp)
                           .arg(levelToString(level))
                           .arg(message);

    if (logStream) {
        *logStream << logEntry;
        logStream->flush();

        if (logFile->size() > maxFileSize) {
            rotateLogFile();
        }
    }

    // Also output to console
    fprintf(stderr, "%s", logEntry.toUtf8().constData());
}

void Logger::rotateLogFile() {
    if (!logFile) return;

    QString baseFilename = logFile->fileName();
    logFile->close();

    // Remove oldest log file if it exists
    QString oldestLog = QString("%1.%2").arg(baseFilename).arg(maxBackupCount);
    QFile::remove(oldestLog);

    // Rotate existing log files
    for (int i = maxBackupCount - 1; i >= 1; --i) {
        QString oldName = QString("%1.%2").arg(baseFilename).arg(i);
        QString newName = QString("%1.%2").arg(baseFilename).arg(i + 1);
        QFile::rename(oldName, newName);
    }

    // Rename current log file
    QFile::rename(baseFilename, baseFilename + ".1");

    // Open new log file
    setLogFile(baseFilename);
}

QString Logger::levelToString(LogLevel level) {
    switch (level) {
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::ERROR: return "ERROR";
    case LogLevel::CRITICAL: return "CRITICAL";
    default: return "UNKNOWN";
    }
}

void Logger::setLogLevel(LogLevel level) {
    currentLevel = level;
}

void Logger::debug(const QString& message) { log(LogLevel::DEBUG, message); }
void Logger::info(const QString& message) { log(LogLevel::INFO, message); }
void Logger::warning(const QString& message) { log(LogLevel::WARNING, message); }
void Logger::error(const QString& message) { log(LogLevel::ERROR, message); }
void Logger::critical(const QString& message) { log(LogLevel::CRITICAL, message); }
