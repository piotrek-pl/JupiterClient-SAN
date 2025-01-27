// ConfigManager.h
/**
 * @file ConfigManager.h
 * @brief Configuration manager class definition
 * @author piotrek-pl
 * @date 2025-01-27 08:24:09
 */

#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <memory>

class ConfigManager : public QObject {
    Q_OBJECT

public:
    static ConfigManager& getInstance();

    struct ConnectionConfig {
        QString host;
        quint16 port;
        int maxReconnectAttempts;
        int reconnectDelay;
        int pingInterval;
        int connectionTimeout;
    };

    struct LogConfig {
        QString level;
        QString file;
        qint64 maxFileSize;
        int maxBackupCount;
    };

    ConnectionConfig getConnectionConfig() const;
    LogConfig getLogConfig() const;
    bool isConfigValid() const;

private:
    ConfigManager();
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    void loadConfig();
    bool validateConfig() const;
    std::unique_ptr<QSettings> settings;
    bool configValid{false};
};
