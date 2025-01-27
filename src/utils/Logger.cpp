/**
 * @file Logger.cpp
 * @brief Logger class implementation
 * @author piotrek-pl
 * @date 2025-01-27 09:13:35
 */

#include "Logger.h"
#include "config/ConfigManager.h"
#include <QDateTime>
#include <QDir>

Logger::Logger() : currentLevel(LogLevel::INFO) {
    initializeFromConfig();
}

Logger::~Logger() {
    if (logStream) logStream->flush();
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::initializeFromConfig() {
    const auto& config = ConfigManager::getInstance();
    auto logConfig = config.getLogConfig();

    setLogFile(logConfig.file);
    maxFileSize = logConfig.maxFileSize;
    maxBackupCount = logConfig.maxBackupCount;
    currentLevel = parseLogLevel(logConfig.level);
}

LogLevel Logger::parseLogLevel(const QString& levelStr) {
    if (levelStr == "DEBUG") return LogLevel::DEBUG;
    if (levelStr == "INFO") return LogLevel::INFO;
    if (levelStr == "WARNING") return LogLevel::WARNING;
    if (levelStr == "ERROR") return LogLevel::ERROR;
    if (levelStr == "CRITICAL") return LogLevel::CRITICAL;
    return LogLevel::INFO;
}

void Logger::ensureDirectoryExists(const QString& filename) {
    QString dirPath = QFileInfo(filename).absolutePath();
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

bool Logger::openLogFile(const QString& filename) {
    logFile = std::make_unique<QFile>(filename);
    if (!logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open log file:" << filename;
        return false;
    }
    logStream = std::make_unique<QTextStream>(logFile.get());
    return true;
}

void Logger::setLogFile(const QString& filename) {
    ensureDirectoryExists(filename);
    openLogFile(filename);
}

QString Logger::createLogEntry(LogLevel level, const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    return QString("[%1] [%2] %3\n")
        .arg(timestamp)
        .arg(levelToString(level))
        .arg(message);
}

void Logger::writeLogEntry(const QString& entry) {
    if (logStream) {
        *logStream << entry;
        logStream->flush();
    }
    fprintf(stderr, "%s", entry.toUtf8().constData());
}

void Logger::checkFileSize() {
    if (logFile && logFile->size() > maxFileSize) {
        rotateLogFile();
    }
}

void Logger::log(LogLevel level, const QString& message) {
    if (level < currentLevel) return;

    QString logEntry = createLogEntry(level, message);
    writeLogEntry(logEntry);
    checkFileSize();
}

void Logger::removeOldestLog(const QString& baseFilename) {
    QString oldestLog = QString("%1.%2").arg(baseFilename).arg(maxBackupCount);
    QFile::remove(oldestLog);
}

void Logger::rotateExistingLogs(const QString& baseFilename) {
    for (int i = maxBackupCount - 1; i >= 1; --i) {
        QString oldName = QString("%1.%2").arg(baseFilename).arg(i);
        QString newName = QString("%1.%2").arg(baseFilename).arg(i + 1);
        QFile::rename(oldName, newName);
    }
}

void Logger::createNewLogFile(const QString& baseFilename) {
    QFile::rename(baseFilename, baseFilename + ".1");
    setLogFile(baseFilename);
}

void Logger::closeCurrentFile() {
    if (logStream) {
        logStream->flush();
    }
    if (logFile) {
        logFile->close();
    }
}

void Logger::rotateLogFile() {
    if (!logFile) return;

    QString baseFilename = logFile->fileName();
    closeCurrentFile();

    removeOldestLog(baseFilename);
    rotateExistingLogs(baseFilename);
    createNewLogFile(baseFilename);
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
