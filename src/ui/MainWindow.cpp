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
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshFriendsListClicked);

    // Status combo box
    ui->statusComboBox->addItem("Online", Protocol::UserStatus::ONLINE);
    ui->statusComboBox->addItem("Away", Protocol::UserStatus::AWAY);
    ui->statusComboBox->addItem("Busy", Protocol::UserStatus::BUSY);
    connect(ui->statusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onStatusChanged);


    // Menu connections
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::onMenuSettingsTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onMenuExitTriggered);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onMenuAboutTriggered);

    // Podwójne kliknięcie na liście znajomych
    connect(ui->friendsList, &QListWidget::itemDoubleClicked,
            this, &MainWindow::openChatWindow);

    updateConnectionStatus("Initializing...");
}

void MainWindow::setupNetworkConnections()
{
    disconnect(&networkManager, nullptr, this, nullptr);

    connect(&networkManager, &NetworkManager::connectionStatusChanged,
            this, &MainWindow::onConnectionStatusChanged);
    connect(&networkManager, &NetworkManager::messageReceived,
            this, &MainWindow::onMessageReceived);
    connect(&networkManager, &NetworkManager::error,
            this, &MainWindow::onNetworkError);
    connect(&networkManager, &NetworkManager::disconnected,
            this, &MainWindow::onDisconnected);

    if (networkManager.isConnected() && networkManager.isAuthenticated()) {
        currentStatus = Protocol::UserStatus::ONLINE;
        updateConnectionStatus("Connected");
    }
}

void MainWindow::onMessageReceived(const QJsonObject& json)
{
    QString type = json["type"].toString();

    if (type == Protocol::MessageType::MESSAGE_RESPONSE) {
        QString sender = json["sender"].toString();
        QString recipient = json["recipient"].toString();
        int senderId = json["senderId"].toInteger();
        int recipientId = json["recipientId"].toInteger();

        // Określ ID okna czatu
        int chatWindowId = (sender == currentUsername) ? recipientId : senderId;

        // Jeśli istnieje okno czatu dla tego użytkownika, przekaż wiadomość
        if (chatWindows.contains(chatWindowId)) {
            chatWindows[chatWindowId]->processMessage(json);
        } else {
            // Jeśli okno nie istnieje, a my jesteśmy odbiorcą
            if (recipient == currentUsername) {
                QString chatPartnerName = sender;
                ChatWindow* chatWindow = new ChatWindow(chatPartnerName, senderId);
                chatWindows[senderId] = chatWindow;

                connect(chatWindow, &ChatWindow::destroyed, [this, senderId]() {
                    chatWindows.remove(senderId);
                });

                chatWindow->processMessage(json);
                chatWindow->show();
                chatWindow->activateWindow();
            }
        }
    }
    else if (type == Protocol::MessageType::FRIENDS_LIST_RESPONSE ||
             type == Protocol::MessageType::FRIENDS_STATUS_UPDATE) {
        updateFriendsList(json["friends"].toArray());
    }
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
        } else if (friendStatus == Protocol::UserStatus::OFFLINE) {
            item->setForeground(Qt::gray);
        } else {
            item->setForeground(Qt::gray);
        }

        ui->friendsList->addItem(item);
    }

    // Zmiana: zachowaj status "Connected" i dodaj informację o liczbie znajomych
    if (networkManager.isConnected() && networkManager.isAuthenticated()) {
        updateConnectionStatus(QString("Connected - Friends list updated (%1)").arg(friends.size()));
    }
}

void MainWindow::onDisconnected()
{
    LOG_WARNING("Disconnected from server");
    updateConnectionStatus("Disconnected from server");
    ui->friendsList->clear();
}

void MainWindow::onConnectionStatusChanged(const QString& status)
{
    if (networkManager.isConnected() && networkManager.isAuthenticated()) {
        updateConnectionStatus("Connected");
    } else {
        updateConnectionStatus(status);
    }
}

void MainWindow::onNetworkError(const QString& error)
{
    LOG_ERROR(QString("Network error: %1").arg(error));
    updateConnectionStatus("Error: " + error);
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
    ui->statusBar->showMessage(status);
}

void MainWindow::openChatWindow(QListWidgetItem* item)
{
    if (!item) return;

    int friendId = item->data(Qt::UserRole).toInt();
    QString friendName = item->text().split(" (").first(); // Usuń status z nazwy

    // Sprawdź czy okno już istnieje
    if (!chatWindows.contains(friendId)) {
        ChatWindow* chatWindow = new ChatWindow(friendName, friendId);
        chatWindows[friendId] = chatWindow;

        // Usuń okno z mapy gdy zostanie zamknięte
        connect(chatWindow, &ChatWindow::destroyed, [this, friendId]() {
            chatWindows.remove(friendId);
        });
    }

    // Pokaż i aktywuj okno
    chatWindows[friendId]->show();
    chatWindows[friendId]->activateWindow();
}
