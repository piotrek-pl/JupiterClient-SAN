/**
 * @file MainWindow.cpp
 * @brief Main window class implementation
 * @author piotrek-pl
 * @date 2025-01-21 13:01:30
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
    , networkManager(NetworkManager::getInstance())
{
    ui->setupUi(this);
    setWindowTitle("Jupiter Client");
    connectionConfig = ConfigManager::getInstance().getConnectionConfig();

    initializeUI();
    setupNetworkConnections();

    LOG_INFO("MainWindow initialized");
}

MainWindow::~MainWindow()
{
    if (networkManager.isConnected()) {
        QJsonObject logoutRequest = Protocol::MessageStructure::createLogoutRequest();
        networkManager.sendMessage(logoutRequest);
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

void MainWindow::setupNetworkConnections()
{
    connect(&networkManager, &NetworkManager::connectionStatusChanged,
            this, &MainWindow::onConnectionStatusChanged);
    connect(&networkManager, &NetworkManager::messageReceived,
            this, &MainWindow::onMessageReceived);
    connect(&networkManager, &NetworkManager::error,
            this, &MainWindow::onNetworkError);
    connect(&networkManager, &NetworkManager::disconnected,
            this, &MainWindow::onDisconnected);

    if (networkManager.isConnected() && networkManager.isAuthenticated()) {
        QTimer::singleShot(100, this, [this]() {
            // Wyślij status online
            currentStatus = Protocol::UserStatus::ONLINE;
            QJsonObject statusUpdate = Protocol::MessageStructure::createStatusUpdate(currentStatus);
            networkManager.sendMessage(statusUpdate);

            // Pobierz listę znajomych
            QJsonObject getFriendsRequest = Protocol::MessageStructure::createGetFriendsList();
            networkManager.sendMessage(getFriendsRequest);
        });
    }
}

void MainWindow::onMessageReceived(const QJsonObject& json)
{
    QString type = json["type"].toString();
    LOG_DEBUG(QString("Processing message type: %1").arg(type));

    if (type == Protocol::MessageType::MESSAGE_RESPONSE) {
        QString sender = json["sender"].toString();
        QString content = json["content"].toString();
        QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(json["timestamp"].toInteger());

        bool isOwn = (sender == currentUsername);
        addMessageToChat(sender, content, timestamp, isOwn);
    }
    else if (type == Protocol::MessageType::FRIENDS_LIST_RESPONSE ||
             type == Protocol::MessageType::FRIENDS_STATUS_UPDATE) {
        updateFriendsList(json["friends"].toArray());
    }
}

void MainWindow::onSendMessageClicked()
{
    if (!networkManager.isConnected() || !networkManager.isAuthenticated()) {
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
    networkManager.sendMessage(messageRequest);

    ui->messageLineEdit->clear();
}

void MainWindow::onStatusChanged(int index)
{
    if (!networkManager.isConnected() || !networkManager.isAuthenticated()) {
        updateConnectionStatus("Cannot update status - not connected");
        return;
    }

    QString newStatus = ui->statusComboBox->itemData(index).toString();
    if (newStatus != currentStatus) {
        currentStatus = newStatus;
        QJsonObject statusUpdate = Protocol::MessageStructure::createStatusUpdate(newStatus);
        networkManager.sendMessage(statusUpdate);
    }
}

void MainWindow::onRefreshFriendsListClicked()
{
    if (!networkManager.isConnected() || !networkManager.isAuthenticated()) {
        updateConnectionStatus("Cannot refresh - not connected");
        return;
    }

    QJsonObject getFriendsRequest = Protocol::MessageStructure::createGetFriendsList();
    networkManager.sendMessage(getFriendsRequest);
}

void MainWindow::updateFriendsList(const QJsonArray& friends)
{
    LOG_INFO(QString("Updating friends list with %1 friends").arg(friends.size()));
    ui->friendsList->clear();

    for (const QJsonValue &friendValue : friends) {
        QJsonObject friendObj = friendValue.toObject();
        QString friendName = friendObj["username"].toString();
        QString friendStatus = friendObj["status"].toString();
        int friendId = friendObj["id"].toInt();

        LOG_DEBUG(QString("Adding friend: %1 (ID: %2) with status: %3")
                      .arg(friendName)
                      .arg(friendId)
                      .arg(friendStatus));

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

    updateConnectionStatus(QString("Friends list updated (%1 friends)").arg(friends.size()));
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
    updateConnectionStatus("Disconnected from server");
    ui->friendsList->clear();
}

void MainWindow::onConnectionStatusChanged(const QString& status)
{
    updateConnectionStatus(status);
}

void MainWindow::onNetworkError(const QString& error)
{
    LOG_ERROR(QString("Network error: %1").arg(error));
    updateConnectionStatus("Error: " + error);
    QMessageBox::warning(this, "Network Error", error);
}

void MainWindow::onMenuSettingsTriggered()
{
    QMessageBox::information(this, "Settings",
                             "Settings functionality will be implemented in future versions.");
}

void MainWindow::onMenuExitTriggered()
{
    if (networkManager.isConnected()) {
        QJsonObject logoutRequest = Protocol::MessageStructure::createLogoutRequest();
        networkManager.sendMessage(logoutRequest);
    }
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
