/**
 * @file NetworkManager.cpp
 * @brief Network manager class implementation
 * @author piotrek-pl
 * @date 2025-01-27 08:29:09
 */

#include "NetworkManager.h"
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>

NetworkManager& NetworkManager::getInstance() {
    static NetworkManager* instance = nullptr;
    if (!instance && QApplication::instance()) {
        instance = new NetworkManager();
        instance->moveToThread(QApplication::instance()->thread());
    }
    return *instance;
}

NetworkManager::NetworkManager()
    : QObject(nullptr)
    , connectionCheckTimer(nullptr)
    , lastPongTime(QDateTime::currentMSecsSinceEpoch())
    , missedPings(0)
    , reconnectAttempts(0)
    , isReconnecting(false)
    , m_isAuthenticated(false)
    , state(Protocol::SessionState::INITIAL)
{
    connectionConfig = ConfigManager::getInstance().getConnectionConfig();
    initializeNetworking();
    LOG_INFO("NetworkManager initialized");
}

NetworkManager::~NetworkManager() {
    if (connectionCheckTimer) {
        connectionCheckTimer->stop();
        delete connectionCheckTimer;
    }

    if (socket.state() == QAbstractSocket::ConnectedState) {
        socket.disconnectFromHost();
    }

    LOG_INFO("NetworkManager destroyed");
}

void NetworkManager::initializeNetworking() {
    connect(&socket, &QTcpSocket::connected, this, &NetworkManager::onConnected);
    connect(&socket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
    connect(&socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    connect(&socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &NetworkManager::onError);

    connectionCheckTimer = new QTimer(this);
    connectionCheckTimer->setInterval(Protocol::Timeouts::PING);
    connect(connectionCheckTimer, &QTimer::timeout, this, &NetworkManager::checkConnection);

    connectionCheckTimer->start();
    LOG_INFO(QString("Connection check timer started with interval: %1 ms").arg(Protocol::Timeouts::PING));
}

void NetworkManager::connectToServer() {
    if (socket.state() == QAbstractSocket::UnconnectedState && !isReconnecting) {
        LOG_INFO(QString("Connecting to %1:%2").arg(connectionConfig.host).arg(connectionConfig.port));
        emitConnectionStatus(QString("Connecting to %1:%2...").arg(connectionConfig.host).arg(connectionConfig.port));
        socket.connectToHost(connectionConfig.host, connectionConfig.port);
    }
}

void NetworkManager::disconnectFromServer() {
    if (socket.state() == QAbstractSocket::ConnectedState) {
        LOG_INFO("Disconnecting from server");
        socket.disconnectFromHost();
    }
}

void NetworkManager::sendMessage(const QJsonObject& message) {
    if (socket.state() != QAbstractSocket::ConnectedState) {
        LOG_WARNING("Attempting to send message while not connected");
        emitConnectionStatus("Not connected to server");
        return;
    }

    QByteArray data = QJsonDocument(message).toJson(QJsonDocument::Compact);
    data.append('\n');
    LOG_INFO("Sending message: " + QString::fromUtf8(data));
    socket.write(data);
    socket.flush();
}

void NetworkManager::login(const QString& username, const QString& password) {
    currentUsername = username;
    currentPassword = password;

    QJsonObject loginRequest = Protocol::MessageStructure::createLoginRequest(username, password);
    sendMessage(loginRequest);
    emitConnectionStatus("Logging in...");
}

void NetworkManager::registerUser(const QString& username, const QString& password, const QString& email) {
    QJsonObject registerRequest = Protocol::MessageStructure::createRegisterRequest(username, password, email);
    sendMessage(registerRequest);
    emitConnectionStatus("Registering...");
}

void NetworkManager::logout() {
    if (m_isAuthenticated) {
        QJsonObject logoutRequest = Protocol::MessageStructure::createLogoutRequest();
        sendMessage(logoutRequest);
        currentUsername.clear();
        currentPassword.clear();
        m_isAuthenticated = false;
    }
}

void NetworkManager::checkConnection() {
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if (socket.state() == QAbstractSocket::ConnectedState) {
        if (currentTime - lastPongTime > Protocol::Timeouts::CONNECTION) {
            handleConnectionTimeout();
        }
    } else if (socket.state() == QAbstractSocket::UnconnectedState && !isReconnecting) {
        connectToServer();
    }
}

void NetworkManager::handleConnectionTimeout() {
    LOG_WARNING(QString("No ping from server for %1 ms").arg(QDateTime::currentMSecsSinceEpoch() - lastPongTime));
    missedPings++;

    if (missedPings >= 3) {
        LOG_WARNING("Connection timeout - disconnecting");
        socket.disconnectFromHost();
        scheduleReconnection();
    }
}

void NetworkManager::scheduleReconnection() {
    if (!isReconnecting) {
        isReconnecting = true;
        reconnectAttempts++;

        if (reconnectAttempts <= MAX_RECONNECT_ATTEMPTS) {
            LOG_INFO(QString("Scheduling reconnection attempt %1/%2")
                         .arg(reconnectAttempts)
                         .arg(MAX_RECONNECT_ATTEMPTS));

            QTimer::singleShot(RECONNECT_DELAY, this, [this]() {
                isReconnecting = false;
                connectToServer();
            });
        } else {
            LOG_ERROR("Max reconnection attempts reached");
            emit error("Failed to reconnect after multiple attempts");
            reconnectAttempts = 0;
            isReconnecting = false;
        }
    }
}

void NetworkManager::onConnected() {
    lastPongTime = QDateTime::currentMSecsSinceEpoch();
    missedPings = 0;
    reconnectAttempts = 0;

    LOG_INFO("Connected to server");
    emitConnectionStatus("Connected to server");
    emit connected();

    if (!currentUsername.isEmpty() && !currentPassword.isEmpty()) {
        LOG_INFO("Attempting automatic re-login");
        login(currentUsername, currentPassword);
    }
}

void NetworkManager::onDisconnected() {
    LOG_WARNING("Disconnected from server");
    emitConnectionStatus("Disconnected from server");
    m_isAuthenticated = false;
    emit disconnected();
    scheduleReconnection();
}

void NetworkManager::handleSocketError(QAbstractSocket::SocketError socketError) {
    QString errorMsg = socket.errorString();
    LOG_ERROR(QString("Socket error: %1 (%2)").arg(socketError).arg(errorMsg));

    QString statusMessage;
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            statusMessage = "Server closed connection - reconnecting...";
            break;
        case QAbstractSocket::ConnectionRefusedError:
            statusMessage = "Connection refused - check if server is running";
            break;
        case QAbstractSocket::HostNotFoundError:
            statusMessage = "Server not found - check server address";
            break;
        case QAbstractSocket::NetworkError:
            statusMessage = "Network error - check your connection";
            break;
        default:
            statusMessage = "Connection error: " + errorMsg;
            break;
    }

    emitConnectionStatus(statusMessage);
    emit error(errorMsg);
}

void NetworkManager::onError(QTcpSocket::SocketError socketError) {
    handleSocketError(socketError);
    m_isAuthenticated = false;
    scheduleReconnection();
}

void NetworkManager::onReadyRead() {
    if (!socket.bytesAvailable()) return;

    buffer.append(socket.readAll());
    processBuffer();
}

void NetworkManager::processBuffer() {
    bool foundJson;
    do {
        foundJson = false;
        int startPos = buffer.indexOf('{');
        if (startPos >= 0) {
            int endPos;
            QJsonObject jsonObject;

            if (parseJsonFromBuffer(startPos, endPos, jsonObject)) {
                processIncomingMessage(jsonObject);
                buffer.remove(0, endPos + 1);
                foundJson = true;
            }
        }
    } while (foundJson);
}

bool NetworkManager::parseJsonFromBuffer(int startPos, int& endPos, QJsonObject& jsonObject) {
    endPos = -1;
    int braceCount = 1;

    for (int i = startPos + 1; i < buffer.size(); ++i) {
        if (buffer[i] == '{') braceCount++;
        else if (buffer[i] == '}') {
            braceCount--;
            if (braceCount == 0) {
                endPos = i;
                break;
            }
        }
    }

    if (endPos > startPos) {
        QByteArray jsonData = buffer.mid(startPos, endPos - startPos + 1);
        LOG_INFO("Processing JSON: " + QString::fromUtf8(jsonData));

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);

        if (error.error == QJsonParseError::NoError) {
            jsonObject = doc.object();
            return true;
        } else {
            LOG_ERROR(QString("JSON parse error: %1").arg(error.errorString()));
        }
    }

    return false;
}

void NetworkManager::handleLoginResponse(const QJsonObject& json) {
    if (json["status"].toString() == "success") {
        LOG_INFO("Login successful");
        m_isAuthenticated = true;
        lastPongTime = QDateTime::currentMSecsSinceEpoch();
        missedPings = 0;

        QJsonObject statusUpdate = Protocol::MessageStructure::createStatusUpdate(Protocol::UserStatus::ONLINE);
        sendMessage(statusUpdate);

        QJsonObject getFriendsRequest = Protocol::MessageStructure::createGetFriendsList();
        sendMessage(getFriendsRequest);

        emitConnectionStatus("Login successful - Connected");
        emit loginSuccessful();
    } else {
        LOG_WARNING(QString("Login failed: %1").arg(json["message"].toString()));
        emitConnectionStatus("Login failed: " + json["message"].toString());
        currentUsername.clear();
        currentPassword.clear();
        m_isAuthenticated = false;
        emit error(json["message"].toString());
    }
}

void NetworkManager::handleRegisterResponse(const QJsonObject& json) {
    if (json["status"].toString() == "success") {
        LOG_INFO("Registration successful");
        emit registrationSuccessful();
        emitConnectionStatus("Registration successful - please login");
    } else {
        LOG_WARNING(QString("Registration failed: %1").arg(json["message"].toString()));
        emitConnectionStatus("Registration failed: " + json["message"].toString());
        emit error(json["message"].toString());
    }
}

void NetworkManager::handleErrorMessage(const QJsonObject& json) {
    QString errorMsg = json["message"].toString();
    LOG_ERROR(QString("Server error: %1").arg(errorMsg));

    if (errorMsg == "Invalid JSON format") {
        LOG_ERROR("Invalid JSON sent to server - check message format");
    }

    emitConnectionStatus("Server error: " + errorMsg);
    emit error(errorMsg);

    if (errorMsg == "Session expired" || errorMsg == "Authentication required") {
        m_isAuthenticated = false;
        emitConnectionStatus("Session expired - please login again");
    }
}

void NetworkManager::handlePingMessage(const QJsonObject& json) {
    qint64 timestamp = json["timestamp"].toInteger();
    LOG_INFO(QString("Received PING (timestamp: %1)").arg(timestamp));
    sendPong(timestamp);
    lastPongTime = QDateTime::currentMSecsSinceEpoch();
    missedPings = 0;
}

void NetworkManager::processIncomingMessage(const QJsonObject& json) {
    QString type = json["type"].toString();
    LOG_DEBUG(QString("Processing message type: %1").arg(type));

    if (type == Protocol::MessageType::ERROR &&
        !m_isAuthenticated &&
        json["message"].toString() == "Not authenticated") {
        LOG_DEBUG("Ignoring 'Not authenticated' error - not logged in yet");
        lastPongTime = QDateTime::currentMSecsSinceEpoch();
        return;
    }

    if (type == Protocol::MessageType::PING) {
        handlePingMessage(json);
        return;
    }

    if (type == Protocol::MessageType::LOGIN_RESPONSE) {
        handleLoginResponse(json);
    }
    else if (type == Protocol::MessageType::REGISTER_RESPONSE) {
        handleRegisterResponse(json);
    }
    else if (type == Protocol::MessageType::ERROR) {
        handleErrorMessage(json);
    }
    // Znane typy wiadomości, które są obsługiwane przez odpowiednie dialogi
    else if (type == Protocol::MessageType::UNREAD_FROM ||
             type == Protocol::MessageType::FRIENDS_LIST_RESPONSE ||
             type == Protocol::MessageType::FRIENDS_STATUS_UPDATE ||
             type == Protocol::MessageType::RECEIVED_INVITATIONS_RESPONSE ||
             type == Protocol::MessageType::SENT_INVITATIONS_RESPONSE) {
        LOG_DEBUG(QString("Message type %1 will be handled by appropriate dialog").arg(type));
    }
    else if (type != Protocol::MessageType::ERROR) {
        LOG_WARNING(QString("Received unknown message type: %1").arg(type));
    }

    emit messageReceived(json);

    if (type != Protocol::MessageType::ERROR || m_isAuthenticated) {
        lastPongTime = QDateTime::currentMSecsSinceEpoch();
    }
}

void NetworkManager::sendPong(qint64 timestamp) {
    QJsonObject pongMessage = Protocol::MessageStructure::createPong(timestamp);
    sendMessage(pongMessage);
    LOG_INFO(QString("Sent PONG (timestamp: %1)").arg(timestamp));
}

void NetworkManager::emitConnectionStatus(const QString& status) {
    static QString lastStatus;
    if (lastStatus != status) {
        lastStatus = status;
        LOG_INFO(QString("Status update: %1").arg(status));
        emit connectionStatusChanged(status);
    }
}
