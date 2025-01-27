// ConfigManager.cpp
/**
 * @file ConfigManager.cpp
 * @brief Configuration manager class implementation
 * @author piotrek-pl
 * @date 2025-01-27 08:24:09
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
        configValid = false;
        return;
    }

    configValid = validateConfig();
    if (!configValid) {
        qWarning() << "Configuration validation failed. Required parameters missing in:" << configPath;
    }
}

bool ConfigManager::validateConfig() const {
    // Sprawdź czy wymagane parametry istnieją w pliku konfiguracyjnym
    return settings->contains("ConnectionSettings/host") &&
           settings->contains("ConnectionSettings/port");
}

bool ConfigManager::isConfigValid() const {
    return configValid;
}

ConfigManager::ConnectionConfig ConfigManager::getConnectionConfig() const {
    if (!configValid) {
        qWarning() << "Configuration is invalid, returning default values";
        return ConnectionConfig{
            "localhost",  // Domyślny host
            1234,        // Domyślny port
            3,           // Domyślne wartości dla pozostałych parametrów
            5000,
            1000,
            5000
        };
    }

    ConnectionConfig config;
    config.host = settings->value("ConnectionSettings/host").toString();
    config.port = settings->value("ConnectionSettings/port").toUInt();
    config.maxReconnectAttempts = settings->value("ConnectionSettings/maxReconnectAttempts", 3).toInt();
    config.reconnectDelay = settings->value("ConnectionSettings/reconnectDelay", 5000).toInt();
    config.pingInterval = settings->value("ConnectionSettings/pingInterval", 1000).toInt();
    config.connectionTimeout = settings->value("ConnectionSettings/connectionTimeout", 5000).toInt();
    return config;
}

ConfigManager::LogConfig ConfigManager::getLogConfig() const {
    if (!configValid) {
        qWarning() << "Configuration is invalid, returning default values";
        return LogConfig{
            "INFO",
            "jupiter_client.log",
            1048576,
            3
        };
    }

    LogConfig config;
    config.level = settings->value("LogSettings/level", "INFO").toString();
    config.file = settings->value("LogSettings/file", "jupiter_client.log").toString();
    config.maxFileSize = settings->value("LogSettings/maxFileSize", 1048576).toLongLong();
    config.maxBackupCount = settings->value("LogSettings/maxBackupCount", 3).toInt();
    return config;
}
