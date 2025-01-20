/**
 * @file LoginWindow.cpp
 * @brief Login window class implementation
 * @author piotrek-pl
 * @date 2025-01-20 14:37:03
 */

#include "LoginWindow.h"
#include "network/Protocol.h"
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
    , state(Protocol::SessionState::INITIAL)  // Dodaj stan początkowy
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

    // Initialize connection check timer
    connectionCheckTimer = new QTimer(this);
    connectionCheckTimer->setInterval(Protocol::Timeouts::PING); // 10 sekund
    connect(connectionCheckTimer, &QTimer::timeout, this, [this]() {
        LOG_INFO("Timer triggered - checking connection");
        checkConnection();
    });

    // Explicitnie startujemy timer
    connectionCheckTimer->start();
    LOG_INFO(QString("Connection check timer started with interval: %1 ms").arg(Protocol::Timeouts::PING));

    // Pierwsza próba połączenia
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

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    // Sprawdzamy połączenie tylko jeśli socket jest połączony
    if (socket.state() == QAbstractSocket::ConnectedState) {
        if (currentTime - lastPongTime > Protocol::Timeouts::CONNECTION) { // 30 sekund zamiast 10
            LOG_WARNING(QString("No ping from server for %1 ms").arg(currentTime - lastPongTime));
            missedPings++;

            if (missedPings >= 3) {  // Dodajemy licznik brakujących pingów
                LOG_WARNING("Connection timeout - disconnecting");
                socket.disconnectFromHost();
                scheduleReconnection();
            }
        }
    } else if (socket.state() == QAbstractSocket::UnconnectedState) {
        LOG_INFO("Socket unconnected - attempting reconnection");
        socket.connectToHost(connectionConfig.host, connectionConfig.port);
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
    while (socket.bytesAvailable() > 0) {
        QByteArray data = socket.readAll();
        LOG_INFO(QString("Received raw data size: %1").arg(data.size()));
        LOG_DEBUG(QString("Raw data: %1").arg(QString(data)));

        if (data.isEmpty()) {
            LOG_WARNING("Empty data received from server");
            continue;
        }

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        if (error.error != QJsonParseError::NoError) {
            LOG_ERROR(QString("JSON parse error: %1").arg(error.errorString()));
            LOG_ERROR(QString("Invalid JSON data: %1").arg(QString(data)));
            continue;
        }

        QJsonObject json = doc.object();
        processIncomingMessage(json);
    }
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

    // Odpowiadamy na PING od serwera
    if (type == Protocol::MessageType::PING) {
        qint64 timestamp = json["timestamp"].toInteger();
        LOG_INFO(QString("CLIENT: Received PING from server at %1 (timestamp: %2)")
                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"))
                     .arg(timestamp));
        updateConnectionStatus(QString("Received ping (timestamp: %1)").arg(timestamp));

        // Wysyłamy PONG
        sendPong(timestamp);
        lastPongTime = QDateTime::currentMSecsSinceEpoch();
        missedPings = 0;
        return;
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
