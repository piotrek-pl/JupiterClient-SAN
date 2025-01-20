/**
 * @file ConfigManager.cpp
 * @brief Configuration manager class implementation
 * @author piotrek-pl
 * @date 2025-01-20 13:36:46
 */

#include "ConfigManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

ConfigManager::ConfigManager() {
    loadConfig();
}

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

void ConfigManager::loadConfig() {
    QString configPath = QCoreApplication::applicationDirPath() + "/config/jupiter.conf";
    settings = std::make_unique<QSettings>(configPath, QSettings::IniFormat);

    if (settings->status() != QSettings::NoError) {
        qWarning() << "Error loading configuration file:" << configPath;
    }
}

ConfigManager::ConnectionConfig ConfigManager::getConnectionConfig() const {
    ConnectionConfig config;
    config.host = settings->value("ConnectionSettings/host", "127.0.0.1").toString();
    config.port = settings->value("ConnectionSettings/port", 1234).toUInt();
    config.maxReconnectAttempts = settings->value("ConnectionSettings/maxReconnectAttempts", 3).toInt();
    config.reconnectDelay = settings->value("ConnectionSettings/reconnectDelay", 5000).toInt();
    config.pingInterval = settings->value("ConnectionSettings/pingInterval", 1000).toInt();
    config.connectionTimeout = settings->value("ConnectionSettings/connectionTimeout", 5000).toInt();
    return config;
}

ConfigManager::LogConfig ConfigManager::getLogConfig() const {
    LogConfig config;
    config.level = settings->value("LogSettings/level", "INFO").toString();
    config.file = settings->value("LogSettings/file", "jupiter_client.log").toString();
    config.maxFileSize = settings->value("LogSettings/maxFileSize", 1048576).toLongLong();
    config.maxBackupCount = settings->value("LogSettings/maxBackupCount", 3).toInt();
    return config;
}
