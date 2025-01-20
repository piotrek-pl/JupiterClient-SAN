/**
 * @file MainWindow.cpp
 * @brief Main window class implementation
 * @author piotrek-pl
 * @date 2025-01-20 14:03:26
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(nullptr)
    , connectionCheckTimer(nullptr)
    , lastPongTime(QDateTime::currentMSecsSinceEpoch())
    , missedPings(0)
    , reconnectAttempts(0)
    , isReconnecting(false)
    , isAuthenticated(false)  // Zmienione z true na false
{
    ui->setupUi(this);
    setWindowTitle("Jupiter Client");
    connectionConfig = ConfigManager::getInstance().getConnectionConfig();
    initializeUI();
    LOG_INFO("MainWindow initialized");
}

MainWindow::~MainWindow()
{
    if (connectionCheckTimer) {
        connectionCheckTimer->stop();
        delete connectionCheckTimer;
    }

    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        QJsonObject logoutRequest = Protocol::MessageStructure::createLogoutRequest();
        socket->write(QJsonDocument(logoutRequest).toJson());
        socket->flush();
        socket->disconnectFromHost();
    }

    delete ui;
    LOG_INFO("MainWindow destroyed");
}

void MainWindow::initializeUI()
{
    // Przyciski
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendMessageClicked);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshFriendsListClicked);

    // Status combo box
    ui->statusComboBox->addItem("Online", Protocol::UserStatus::ONLINE);
    ui->statusComboBox->addItem("Away", Protocol::UserStatus::AWAY);
    ui->statusComboBox->addItem("Busy", Protocol::UserStatus::BUSY);
    connect(ui->statusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onStatusChanged);

    // Chat
    ui->chatTextEdit->setReadOnly(true);

    // Message input
    ui->messageLineEdit->setPlaceholderText("Type your message here...");
    connect(ui->messageLineEdit, &QLineEdit::returnPressed, this, &MainWindow::onSendMessageClicked);

    // Menu connections
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::onMenuSettingsTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onMenuExitTriggered);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onMenuAboutTriggered);

    updateConnectionStatus("Initializing...");
}

void MainWindow::setSocket(QTcpSocket *sharedSocket)
{
    socket = sharedSocket;
    if (socket) {
        initializeNetworking();
        LOG_INFO("Socket set and networking initialized in MainWindow");
    } else {
        LOG_ERROR("Attempted to set null socket in MainWindow");
    }
}


void MainWindow::initializeNetworking()
{
    if (!socket) return;

    // Socket connections
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &MainWindow::onError);

    // Initialize connection check timer
    connectionCheckTimer = new QTimer(this);
    connect(connectionCheckTimer, &QTimer::timeout, this, &MainWindow::checkConnection);
    connectionCheckTimer->start(connectionConfig.pingInterval);

    LOG_INFO("MainWindow networking initialized");
}

void MainWindow::checkConnection()
{
    if (!socket || !socket->isValid()) {
        return;
    }

    LOG_DEBUG(QString("Checking connection... Socket state: %1").arg(socket->state()));

    if (socket->state() != QAbstractSocket::ConnectedState) {
        LOG_WARNING("Socket disconnected");
        updateConnectionStatus("Connection lost - attempting to reconnect...");
        scheduleReconnection();
    } else if (isAuthenticated) {
        // Sprawdzamy tylko timeout ostatniej odpowiedzi od serwera
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        if (currentTime - lastPongTime > connectionConfig.connectionTimeout) {
            missedPings++;
            LOG_WARNING(QString("No response from server for %1 ms").arg(currentTime - lastPongTime));

            if (missedPings >= 3) {
                LOG_WARNING("Connection timeout - reconnecting...");
                socket->disconnectFromHost();
                scheduleReconnection();
                return;
            }
        }
    }
}

void MainWindow::scheduleReconnection()
{
    if (!isReconnecting) {
        isReconnecting = true;
        reconnectAttempts = 0;
        missedPings = 0;

        LOG_INFO("Scheduling reconnection attempt");
        QTimer::singleShot(connectionConfig.reconnectDelay, this, [this]() {
            isReconnecting = false;
            if (socket && socket->state() != QAbstractSocket::ConnectedState) {
                socket->connectToHost(connectionConfig.host, connectionConfig.port);
            }
        });
    }
}

void MainWindow::onReadyRead()
{
    if (!socket) return;

    QByteArray data = socket->readAll();
    LOG_DEBUG(QString("Received data size: %1 bytes").arg(data.size()));

    if (data.isEmpty()) {
        LOG_WARNING("Empty data received from server");
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        LOG_ERROR(QString("JSON parse error: %1").arg(error.errorString()));
        return;
    }

    QJsonObject json = doc.object();
    processIncomingMessage(json);
}

void MainWindow::processIncomingMessage(const QJsonObject& json)
{
    QString type = json["type"].toString();
    LOG_DEBUG(QString("Processing message type: %1").arg(type));

    if (type == Protocol::MessageType::LOGIN_RESPONSE) {
        if (json["status"].toString() == "success") {
            isAuthenticated = true;
            currentUsername = json["username"].toString();
            LOG_INFO("Successfully logged in");

            // Po zalogowaniu wysyłamy żądanie o listę znajomych
            QJsonObject getFriendsRequest = Protocol::MessageStructure::createGetFriendsList();
            socket->write(QJsonDocument(getFriendsRequest).toJson());
            socket->flush();
        }
    }
    else if (type == Protocol::MessageType::PING && isAuthenticated) {
        LOG_DEBUG("Ping received from server");
        sendPong(json["timestamp"].toInteger());
    }
    else if (type == Protocol::MessageType::PONG && isAuthenticated) {
        LOG_DEBUG("Pong received from server");
        lastPongTime = QDateTime::currentMSecsSinceEpoch();
        missedPings = 0;
    }
    else if (type == Protocol::MessageType::MESSAGE_RESPONSE) {
        QString sender = json["sender"].toString();
        QString content = json["content"].toString();
        QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(json["timestamp"].toInteger());

        bool isOwn = (sender == currentUsername);
        addMessageToChat(sender, content, timestamp, isOwn);
    }
    else if (type == Protocol::MessageType::FRIENDS_LIST_RESPONSE) {
        updateFriendsList(json["friends"].toArray());
    }
    else if (type == Protocol::MessageType::FRIENDS_STATUS_UPDATE) {
        updateFriendsList(json["friends"].toArray());
    }
    else if (type == Protocol::MessageType::ERROR) {
        if (!isAuthenticated && json["message"].toString() == "Not authenticated") {
            // Ignoruj błędy autentykacji przed zalogowaniem
            return;
        }
        LOG_ERROR(QString("Server error: %1").arg(json["message"].toString()));
        updateConnectionStatus("Server error: " + json["message"].toString());
    }
    else {
        LOG_WARNING(QString("Unknown message type received: %1").arg(type));
    }
}

void MainWindow::sendPong(qint64 timestamp)
{
    if (!socket || !isAuthenticated) return;

    QJsonObject pongMessage = Protocol::MessageStructure::createPong(timestamp);
    QByteArray data = QJsonDocument(pongMessage).toJson();

    LOG_DEBUG(QString("Sending pong with timestamp: %1").arg(timestamp));
    socket->write(data);
    socket->flush();
}

void MainWindow::onSendMessageClicked()
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        updateConnectionStatus("Cannot send message - not connected");
        return;
    }

    QString message = ui->messageLineEdit->text().trimmed();
    if (message.isEmpty()) {
        return;
    }

    QListWidgetItem* currentItem = ui->friendsList->currentItem();
    if (!currentItem) {
        updateConnectionStatus("Please select a friend to send message to");
        return;
    }

    int selectedFriendId = currentItem->data(Qt::UserRole).toInt();
    if (selectedFriendId == 0) {
        updateConnectionStatus("Please select a valid friend to send message to");
        return;
    }

    QJsonObject messageRequest = Protocol::MessageStructure::createMessage(selectedFriendId, message);
    socket->write(QJsonDocument(messageRequest).toJson());

    ui->messageLineEdit->clear();
}

void MainWindow::onStatusChanged(int index)
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        updateConnectionStatus("Cannot update status - not connected");
        return;
    }

    QString newStatus = ui->statusComboBox->itemData(index).toString();
    if (newStatus != currentStatus) {
        currentStatus = newStatus;
        QJsonObject statusUpdate = Protocol::MessageStructure::createStatusUpdate(newStatus);
        socket->write(QJsonDocument(statusUpdate).toJson());
    }
}

void MainWindow::onRefreshFriendsListClicked()
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        updateConnectionStatus("Cannot refresh - not connected");
        return;
    }

    QJsonObject getFriendsRequest = Protocol::MessageStructure::createGetFriendsList();
    socket->write(QJsonDocument(getFriendsRequest).toJson());
}

void MainWindow::updateFriendsList(const QJsonArray& friends)
{
    ui->friendsList->clear();

    for (const QJsonValue &friendValue : friends) {
        QJsonObject friendObj = friendValue.toObject();
        QString friendName = friendObj["username"].toString();
        QString friendStatus = friendObj["status"].toString();
        int friendId = friendObj["id"].toInt();

        QString displayText = QString("%1 (%2)").arg(friendName).arg(friendStatus);

        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, friendId);

        if (friendStatus == Protocol::UserStatus::ONLINE) {
            item->setForeground(Qt::green);
        } else if (friendStatus == Protocol::UserStatus::AWAY) {
            item->setForeground(Qt::yellow);
        } else if (friendStatus == Protocol::UserStatus::BUSY) {
            item->setForeground(Qt::red);
        } else {
            item->setForeground(Qt::gray);
        }

        ui->friendsList->addItem(item);
    }
}

void MainWindow::addMessageToChat(const QString& sender, const QString& content,
                                  const QDateTime& timestamp, bool isOwn)
{
    QString timeStr = timestamp.toString("HH:mm:ss");
    QString messageHtml;

    if (isOwn) {
        messageHtml = QString("<div style='text-align: right;'><b>%1</b> [%2]<br>%3</div>")
        .arg(sender)
            .arg(timeStr)
            .arg(content);
    } else {
        messageHtml = QString("<div style='text-align: left;'><b>%1</b> [%2]<br>%3</div>")
        .arg(sender)
            .arg(timeStr)
            .arg(content);
    }

    ui->chatTextEdit->append(messageHtml);

    // Przewiń do najnowszej wiadomości
    QScrollBar *scrollBar = ui->chatTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void MainWindow::onDisconnected()
{
    LOG_WARNING("Disconnected from server");
    updateConnectionStatus("Disconnected from server - attempting to reconnect...");
    isAuthenticated = false;
    scheduleReconnection();
    // Wyczyść listę znajomych przy rozłączeniu
    ui->friendsList->clear();
}

void MainWindow::onError(QTcpSocket::SocketError socketError)
{
    if (!socket) return;

    QString errorMsg = socket->errorString();
    LOG_ERROR(QString("Socket error: %1 (%2)").arg(socketError).arg(errorMsg));

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        LOG_WARNING("Server closed connection");
        updateConnectionStatus("Server closed connection - reconnecting...");
        break;

    case QAbstractSocket::ConnectionRefusedError:
        LOG_ERROR("Connection refused by server");
        updateConnectionStatus("Connection refused - check server status");
        break;

    default:
        updateConnectionStatus("Connection error: " + errorMsg);
        break;
    }

    isAuthenticated = false;
    scheduleReconnection();
}

void MainWindow::onMenuSettingsTriggered()
{
    // TODO: Implement settings dialog
    QMessageBox::information(this, "Settings",
                             "Settings functionality will be implemented in future versions.");
}

void MainWindow::onMenuExitTriggered()
{
    // Wyślij informację o wylogowaniu przed zamknięciem
    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        QJsonObject logoutRequest = Protocol::MessageStructure::createLogoutRequest();
        socket->write(QJsonDocument(logoutRequest).toJson());
        socket->flush();
    }

    // Zamknij aplikację
    QApplication::quit();
}

void MainWindow::onMenuAboutTriggered()
{
    QString aboutText = QString(
                            "Jupiter Client v1.0.0\n\n"
                            "Created by: piotrek-pl\n"
                            "Build date: %1 %2\n\n"
                            "A modern chat client for secure communication."
                            ).arg(__DATE__).arg(__TIME__);

    QMessageBox::about(this, "About Jupiter Client", aboutText);
}

void MainWindow::updateConnectionStatus(const QString& status)
{
    LOG_INFO(QString("Status update: %1").arg(status));
    ui->statusBar->showMessage(status);
}
