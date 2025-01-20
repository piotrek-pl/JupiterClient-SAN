/**
 * @file LoginWindow.cpp
 * @brief Login window class implementation
 * @author piotrek-pl
 * @date 2025-01-20 14:37:03
 */

#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include <QJsonDocument>
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWindow)
    , lastPongTime(QDateTime::currentMSecsSinceEpoch())
    , missedPings(0)
    , reconnectAttempts(0)
    , isReconnecting(false)
    , isAuthenticated(false)
{
    ui->setupUi(this);

    setWindowTitle("Jupiter Client - Login");

    // Pobierz konfigurację
    connectionConfig = ConfigManager::getInstance().getConnectionConfig();

    initializeUI();
    initializeNetworking();

    LOG_INFO("LoginWindow initialized");
}

LoginWindow::~LoginWindow()
{
    if (connectionCheckTimer) {
        connectionCheckTimer->stop();
        delete connectionCheckTimer;
    }

    if (socket.state() == QAbstractSocket::ConnectedState) {
        socket.disconnectFromHost();
    }

    delete ui;
    LOG_INFO("LoginWindow destroyed");
}

void LoginWindow::initializeUI()
{
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginButtonClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterButtonClicked);

    // Ustaw placeholdery
    ui->usernameLineEdit->setPlaceholderText("Enter username");
    ui->passwordLineEdit->setPlaceholderText("Enter password");
    ui->emailLineEdit->setPlaceholderText("Enter email for registration");

    // Ukryj pole email na początku
    ui->emailLineEdit->setVisible(false);
    ui->emailLabel->setVisible(false);

    // Dodaj walidację
    ui->usernameLineEdit->setMaxLength(32);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    updateConnectionStatus("Connecting to server...");
}

void LoginWindow::initializeNetworking()
{
    // Socket connections
    connect(&socket, &QTcpSocket::connected, this, &LoginWindow::onConnected);
    connect(&socket, &QTcpSocket::disconnected, this, &LoginWindow::onDisconnected);
    connect(&socket, &QTcpSocket::readyRead, this, &LoginWindow::onReadyRead);
    connect(&socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &LoginWindow::onError);

    // Ustaw opcje socketa
    socket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    socket.setSocketOption(QAbstractSocket::LowDelayOption, 1);

    // Initialize connection check timer
    connectionCheckTimer = new QTimer(this);
    connect(connectionCheckTimer, &QTimer::timeout, this, &LoginWindow::checkConnection);
    connectionCheckTimer->start(connectionConfig.pingInterval);

    // Wykonaj pierwszą próbę połączenia bezpośrednio
    LOG_INFO(QString("Initiating first connection attempt to %1:%2")
                 .arg(connectionConfig.host)
                 .arg(connectionConfig.port));

    socket.connectToHost(connectionConfig.host, connectionConfig.port);
    updateConnectionStatus(QString("Connecting to %1:%2...")
                               .arg(connectionConfig.host)
                               .arg(connectionConfig.port));
}

void LoginWindow::checkConnection()
{
    LOG_DEBUG(QString("Checking connection... Socket state: %1").arg(socket.state()));

    // Przenosimy deklaracje zmiennych przed switch
    qint64 currentTime;
    QJsonObject pingMessage;
    QByteArray data;

    switch(socket.state()) {
    case QAbstractSocket::UnconnectedState:
        LOG_DEBUG("Socket state: UnconnectedState");
        if (reconnectAttempts < connectionConfig.maxReconnectAttempts) {
            LOG_INFO(QString("Attempting to connect to %1:%2 (attempt %3/%4)")
                         .arg(connectionConfig.host)
                         .arg(connectionConfig.port)
                         .arg(reconnectAttempts + 1)
                         .arg(connectionConfig.maxReconnectAttempts));

            socket.connectToHost(connectionConfig.host, connectionConfig.port);
            reconnectAttempts++;

            updateConnectionStatus(QString("Connecting... (attempt %1/%2)")
                                       .arg(reconnectAttempts)
                                       .arg(connectionConfig.maxReconnectAttempts));
        } else {
            LOG_ERROR("Max reconnection attempts reached");
            updateConnectionStatus("Failed to connect. Please try again later.");
            connectionCheckTimer->stop();
        }
        break;

    case QAbstractSocket::ConnectingState:
        LOG_DEBUG("Socket state: ConnectingState - Connection in progress...");
        break;

    case QAbstractSocket::ConnectedState:
        LOG_DEBUG("Socket state: ConnectedState - Checking ping status...");
        currentTime = QDateTime::currentMSecsSinceEpoch();

        // Debug info about ping timing
        LOG_DEBUG(QString("Current time: %1, Last pong time: %2, Diff: %3 ms")
                      .arg(currentTime)
                      .arg(lastPongTime)
                      .arg(currentTime - lastPongTime));

        if (!isAuthenticated) {
            // Jeśli nie jesteśmy zalogowani, nie wysyłamy pingów
            LOG_DEBUG("Not sending ping - not authenticated");
            lastPongTime = currentTime; // Reset timer
            missedPings = 0;
            break;
        }

        if (currentTime - lastPongTime > connectionConfig.connectionTimeout) {
            missedPings++;
            LOG_WARNING(QString("Missed ping count: %1").arg(missedPings));

            if (missedPings >= 3) {
                LOG_WARNING("Connection timeout - reconnecting...");
                socket.disconnectFromHost();
                scheduleReconnection();
                return;
            }
        }

        // Wysyłamy ping tylko gdy jesteśmy zalogowani
        pingMessage = Protocol::MessageStructure::createPing();
        data = QJsonDocument(pingMessage).toJson();

        LOG_INFO("Sending ping to server");
        updateConnectionStatus("Sending ping to server...");
        socket.write(data);
        socket.flush();
        break;

    default:
        LOG_WARNING(QString("Unexpected socket state: %1").arg(socket.state()));
        break;
    }
}

void LoginWindow::scheduleReconnection()
{
    if (!isReconnecting) {
        isReconnecting = true;
        reconnectAttempts = 0;
        missedPings = 0;
        isAuthenticated = false;

        LOG_INFO("Scheduling reconnection attempt");
        QTimer::singleShot(connectionConfig.reconnectDelay, this, [this]() {
            isReconnecting = false;
            checkConnection();
        });
    }
}

void LoginWindow::onConnected()
{
    LOG_INFO("Connected to server");
    updateConnectionStatus("Connected to server");
    lastPongTime = QDateTime::currentMSecsSinceEpoch();
    missedPings = 0;
    reconnectAttempts = 0;

    // Jeśli były zapisane dane logowania, spróbuj zalogować ponownie
    if (!currentUsername.isEmpty() && !currentPassword.isEmpty()) {
        LOG_INFO("Attempting automatic re-login");
        QJsonObject loginRequest = Protocol::MessageStructure::createLoginRequest(
            currentUsername, currentPassword);
        socket.write(QJsonDocument(loginRequest).toJson());
    }
}

void LoginWindow::onDisconnected()
{
    LOG_WARNING("Disconnected from server");
    updateConnectionStatus("Disconnected from server");
    isAuthenticated = false;
    scheduleReconnection();
}

void LoginWindow::onError(QTcpSocket::SocketError socketError)
{
    QString errorMsg = socket.errorString();
    LOG_ERROR(QString("Socket error: %1 (%2)").arg(socketError).arg(errorMsg));

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        LOG_WARNING("Server closed connection");
        updateConnectionStatus("Server closed connection - reconnecting...");
        break;

    case QAbstractSocket::ConnectionRefusedError:
        LOG_ERROR("Connection refused by server");
        updateConnectionStatus("Connection refused - check if server is running");
        break;

    case QAbstractSocket::HostNotFoundError:
        LOG_ERROR("Host not found");
        updateConnectionStatus("Server not found - check server address");
        break;

    case QAbstractSocket::NetworkError:
        LOG_ERROR("Network error");
        updateConnectionStatus("Network error - check your connection");
        break;

    default:
        updateConnectionStatus("Connection error: " + errorMsg);
        break;
    }

    isAuthenticated = false;
    scheduleReconnection();
}

void LoginWindow::onReadyRead()
{
    QByteArray data = socket.readAll();
    LOG_INFO(QString("Received raw data: %1").arg(QString(data))); // Debug raw data
    LOG_DEBUG(QString("Received data size: %1 bytes").arg(data.size()));

    if (data.isEmpty()) {
        LOG_WARNING("Empty data received from server");
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        LOG_ERROR(QString("JSON parse error: %1").arg(error.errorString()));
        LOG_ERROR(QString("Received invalid JSON data: %1").arg(QString(data)));
        return;
    }

    QJsonObject json = doc.object();
    LOG_DEBUG(QString("Parsed JSON: %1").arg(QString(QJsonDocument(json).toJson()))); // Debug parsed JSON
    processIncomingMessage(json);
}

void LoginWindow::processIncomingMessage(const QJsonObject& json)
{
    QString type = json["type"].toString();
    LOG_DEBUG(QString("Processing message type: %1").arg(type));

    // Ignorujemy błąd "Not authenticated" przed zalogowaniem
    if (type == Protocol::MessageType::ERROR &&
        !isAuthenticated &&
        json["message"].toString() == "Not authenticated") {
        LOG_DEBUG("Ignoring 'Not authenticated' error - not logged in yet");
        lastPongTime = QDateTime::currentMSecsSinceEpoch(); // Resetujemy timer
        return;
    }

    if (type == Protocol::MessageType::PING) {
        qint64 timestamp = json["timestamp"].toInteger();
        LOG_INFO(QString("Ping received from server (timestamp: %1)").arg(timestamp));
        updateConnectionStatus(QString("Received ping (timestamp: %1)").arg(timestamp));
        sendPong(timestamp);
        lastPongTime = QDateTime::currentMSecsSinceEpoch();
        missedPings = 0;
    }
    else if (type == Protocol::MessageType::PONG) {
        qint64 timestamp = json["timestamp"].toInteger();
        LOG_INFO(QString("Pong received from server (timestamp: %1)").arg(timestamp));
        lastPongTime = QDateTime::currentMSecsSinceEpoch();
        missedPings = 0;
    }
    else if (type == Protocol::MessageType::LOGIN_RESPONSE) {
        if (json["status"].toString() == "success") {
            LOG_INFO("Login successful");
            isAuthenticated = true;
            lastPongTime = QDateTime::currentMSecsSinceEpoch();
            missedPings = 0;
            updateConnectionStatus("Login successful - Connected");
            emit loginSuccessful();
        } else {
            LOG_WARNING(QString("Login failed: %1").arg(json["message"].toString()));
            updateConnectionStatus("Login failed: " + json["message"].toString());
            currentUsername.clear();
            currentPassword.clear();
            isAuthenticated = false;
            lastPongTime = QDateTime::currentMSecsSinceEpoch(); // Reset timer on failed login
        }
    }
    else if (type == Protocol::MessageType::REGISTER_RESPONSE) {
        if (json["status"].toString() == "success") {
            LOG_INFO("Registration successful");
            emit registrationSuccessful();
            ui->emailLineEdit->setVisible(false);
            ui->emailLabel->setVisible(false);
            updateConnectionStatus("Registration successful - please login");
            lastPongTime = QDateTime::currentMSecsSinceEpoch(); // Reset timer after registration
        } else {
            LOG_WARNING(QString("Registration failed: %1").arg(json["message"].toString()));
            updateConnectionStatus("Registration failed: " + json["message"].toString());
        }
    }
    else if (type == Protocol::MessageType::ERROR) {
        // Ogólna obsługa błędów (gdy już jesteśmy zalogowani lub to nie jest błąd autentykacji)
        QString errorMsg = json["message"].toString();
        LOG_ERROR(QString("Server error: %1").arg(errorMsg));

        if (errorMsg == "Invalid JSON format") {
            LOG_ERROR("Invalid JSON sent to server - check message format");
        }

        updateConnectionStatus("Server error: " + errorMsg);

        if (errorMsg == "Session expired" || errorMsg == "Authentication required") {
            isAuthenticated = false;
            lastPongTime = QDateTime::currentMSecsSinceEpoch(); // Reset timer on auth errors
            updateConnectionStatus("Session expired - please login again");
        }
    }
    else {
        LOG_WARNING(QString("Unknown message type received: %1").arg(type));
        LOG_DEBUG(QString("Full message content: %1")
                      .arg(QString(QJsonDocument(json).toJson(QJsonDocument::Indented))));
    }

    // Update last activity time for any valid message
    if (type != Protocol::MessageType::ERROR || isAuthenticated) {
        lastPongTime = QDateTime::currentMSecsSinceEpoch();
    }
}

void LoginWindow::sendPong(qint64 timestamp)
{
    QJsonObject pongMessage = Protocol::MessageStructure::createPong(timestamp);
    QByteArray data = QJsonDocument(pongMessage).toJson();

    LOG_INFO(QString("Sending pong response (timestamp: %1)").arg(timestamp));
    updateConnectionStatus(QString("Sent pong response (timestamp: %1)").arg(timestamp));
    socket.write(data);
    socket.flush();
}

void LoginWindow::onLoginButtonClicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        updateConnectionStatus("Please enter both username and password");
        return;
    }

    if (socket.state() != QAbstractSocket::ConnectedState) {
        updateConnectionStatus("Not connected to server - trying to reconnect...");
        checkConnection();
        return;
    }

    currentUsername = username;
    currentPassword = password;

    LOG_INFO(QString("Attempting login for user: %1").arg(username));
    QJsonObject loginRequest = Protocol::MessageStructure::createLoginRequest(username, password);
    socket.write(QJsonDocument(loginRequest).toJson());
    updateConnectionStatus("Logging in...");
}

void LoginWindow::onRegisterButtonClicked()
{
    if (ui->emailLineEdit->isVisible()) {
        // Próba rejestracji
        QString username = ui->usernameLineEdit->text().trimmed();
        QString password = ui->passwordLineEdit->text();
        QString email = ui->emailLineEdit->text().trimmed();

        if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
            updateConnectionStatus("Please fill all fields");
            return;
        }

        if (socket.state() != QAbstractSocket::ConnectedState) {
            updateConnectionStatus("Not connected to server - trying to reconnect...");
            checkConnection();
            return;
        }

        LOG_INFO(QString("Attempting registration for user: %1").arg(username));
        QJsonObject registerRequest = Protocol::MessageStructure::createRegisterRequest(
            username, password, email);
        socket.write(QJsonDocument(registerRequest).toJson());
        updateConnectionStatus("Registering...");
    } else {
        // Pokaż pole email
        ui->emailLineEdit->setVisible(true);
        ui->emailLabel->setVisible(true);
        ui->registerButton->setText("Complete Registration");
        updateConnectionStatus("Please fill all fields to register");
    }
}

void LoginWindow::updateConnectionStatus(const QString& status)
{
    LOG_INFO(QString("Status update: %1").arg(status));
    ui->statusLabel->setText(status);
}
