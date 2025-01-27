/**
 * @file NetworkManager.h
 * @brief Network manager class definition
 * @author piotrek-pl
 * @date 2025-01-27 08:29:09
 */

#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QDateTime>
#include <QJsonObject>
#include <QByteArray>
#include "config/ConfigManager.h"
#include "utils/Logger.h"
#include "Protocol.h"

class NetworkManager : public QObject {
    Q_OBJECT

public:
    static NetworkManager& getInstance();

    // Connection management
    void connectToServer();
    void disconnectFromServer();
    void sendMessage(const QJsonObject& message);
    bool isConnected() const { return socket.state() == QAbstractSocket::ConnectedState; }
    bool isAuthenticated() const { return m_isAuthenticated; }

    // Authentication
    void login(const QString& username, const QString& password);
    void registerUser(const QString& username, const QString& password, const QString& email);
    void logout();
    QString getUsername() const { return currentUsername; }

signals:
    void connected();
    void disconnected();
    void messageReceived(const QJsonObject& message);
    void loginSuccessful();
    void registrationSuccessful();
    void error(const QString& error);
    void connectionStatusChanged(const QString& status);

private:
    NetworkManager();
    ~NetworkManager();
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;

    // Initialization
    void initializeNetworking();

    // Connection management
    void checkConnection();
    void handleConnectionTimeout();
    void scheduleReconnection();
    void emitConnectionStatus(const QString& status);

    // Message processing
    void processBuffer();
    void processIncomingMessage(const QJsonObject& json);
    bool parseJsonFromBuffer(int startPos, int& endPos, QJsonObject& jsonObject);
    void handleLoginResponse(const QJsonObject& json);
    void handleRegisterResponse(const QJsonObject& json);
    void handleErrorMessage(const QJsonObject& json);
    void handlePingMessage(const QJsonObject& json);
    void sendPong(qint64 timestamp);

    // Socket handling
    void handleSocketError(QAbstractSocket::SocketError socketError);

    QTcpSocket socket;
    QTimer* connectionCheckTimer;
    QByteArray buffer;
    qint64 lastPongTime;
    int missedPings;
    int reconnectAttempts;
    QString currentUsername;
    QString currentPassword;
    QString state;
    bool isReconnecting;
    bool m_isAuthenticated;
    ConfigManager::ConnectionConfig connectionConfig;
    static const int MAX_RECONNECT_ATTEMPTS = 5;
    static const int RECONNECT_DELAY = 5000; // ms

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QTcpSocket::SocketError socketError);
};
