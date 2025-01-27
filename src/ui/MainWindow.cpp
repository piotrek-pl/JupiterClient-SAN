/**
 * @file MainWindow.cpp
 * @brief Main window class implementation
 * @author piotrek-pl
 * @date 2025-01-27 09:07:44
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SearchDialog.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(NetworkManager::getInstance())
    , searchDialog(nullptr)
    , invitationsDialog(nullptr)
{
    ui->setupUi(this);
    setWindowTitle(DEFAULT_WINDOW_TITLE);
    connectionConfig = ConfigManager::getInstance().getConnectionConfig();

    initializeUI();
    setupNetworkConnections();
    setupInvitationsMenu();

    LOG_INFO("MainWindow initialized");
}

MainWindow::~MainWindow()
{
    sendLogoutRequest();
    delete searchDialog;
    delete ui;
    LOG_INFO("MainWindow destroyed");
}

// Initialization methods
void MainWindow::setupStatusComboBox()
{
    ui->statusComboBox->clear();
    ui->statusComboBox->addItem(QIcon(":/resources/icons/status_online.svg"), "Online", Protocol::UserStatus::ONLINE);
    ui->statusComboBox->addItem(QIcon(":/resources/icons/status_away.svg"), "Away", Protocol::UserStatus::AWAY);
    ui->statusComboBox->addItem(QIcon(":/resources/icons/status_busy.svg"), "Busy", Protocol::UserStatus::BUSY);
    ui->statusComboBox->setIconSize(QSize(16, 16));

    for (int i = 0; i < ui->statusComboBox->count(); i++) {
        ui->statusComboBox->setItemData(i, ui->statusComboBox->itemText(i).toLower(), Qt::UserRole + 1);
    }
}

void MainWindow::setupFriendsList()
{
    ui->friendsList->setIconSize(QSize(24, 24));
    ui->friendsList->setSpacing(0);
    ui->friendsList->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::setupUIConnections()
{
    connect(ui->statusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onStatusChanged);
    connect(ui->actionSearch, &QAction::triggered, this, &MainWindow::onMenuSearchTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onMenuExitTriggered);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onMenuAboutTriggered);
    connect(ui->friendsList, &QListWidget::itemDoubleClicked, this, &MainWindow::openChatWindow);
    connect(ui->friendsList, &QWidget::customContextMenuRequested,
            this, &MainWindow::showFriendsContextMenu);
}

void MainWindow::initializeUI()
{
    setupStatusComboBox();
    setupFriendsList();
    setupUIConnections();
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

// Message handling methods
void MainWindow::handleSearchResponse(const QJsonObject& json)
{
    if (searchDialog) {
        searchDialog->onSearchResponse(json);
    }
}

void MainWindow::handleFriendRequest(const QJsonObject& json)
{
    int fromUserId = json["from_user_id"].toInt();
    QString username = json["username"].toString();

    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Friend Request",
                                                              QString("%1 wants to add you to their friends list. Accept?").arg(username),
                                                              QMessageBox::Yes | QMessageBox::No);

    QJsonObject response;
    if (reply == QMessageBox::Yes) {
        response = Protocol::MessageStructure::createFriendRequestAccept(fromUserId);
        LOG_INFO(QString("Accepted friend request from user %1").arg(username));
    } else {
        response = Protocol::MessageStructure::createFriendRequestReject(fromUserId);
        LOG_INFO(QString("Rejected friend request from user %1").arg(username));
    }

    networkManager.sendMessage(response);
    refreshInvitationsDialog();
}

void MainWindow::handleFriendRequestAcceptResponse(const QJsonObject& json)
{
    if (json["status"].toString() == "success") {
        QMessageBox::information(this, "Success", "Friend added successfully!");
        QJsonObject getFriendsRequest = Protocol::MessageStructure::createGetFriendsList();
        networkManager.sendMessage(getFriendsRequest);
        refreshInvitationsDialog();
        LOG_INFO("Friend request accepted successfully");
    } else {
        QString errorMessage = json["message"].toString();
        QMessageBox::warning(this, "Error", "Failed to add friend: " + errorMessage);
        LOG_WARNING(QString("Failed to add friend: %1").arg(errorMessage));
    }
}

void MainWindow::handleFriendRequestRejectResponse(const QJsonObject& json)
{
    if (json["status"].toString() == "success") {
        LOG_INFO("Friend request rejected successfully");
        refreshInvitationsDialog();
    } else {
        QString errorMessage = json["message"].toString();
        LOG_WARNING(QString("Failed to reject friend request: %1").arg(errorMessage));
    }
}

void MainWindow::handleUnreadMessages(const QJsonObject& json)
{
    QJsonArray unreadFrom = json["users"].toArray();
    LOG_INFO(QString("Received unread messages info from %1 users").arg(unreadFrom.size()));

    for (const QJsonValue &userValue : unreadFrom) {
        QJsonObject userObj = userValue.toObject();
        int userId = userObj["id"].toString().toInt();
        unreadMessagesMap[userId] = true;
    }
}

void MainWindow::handleLoginResponse(const QJsonObject& json)
{
    if (json["status"].toString() == "success") {
        currentUsername = json["username"].toString();
        if (json.contains("friends")) {
            updateFriendsList(json["friends"].toArray());
        }
        refreshInvitationsDialog();
    }
}

void MainWindow::handleLatestMessages(const QJsonObject& json)
{
    QJsonArray messages = json["messages"].toArray();
    for (const QJsonValue &messageValue : messages) {
        QJsonObject messageObj = messageValue.toObject();
        int fromId = messageObj["from"].toInt();
        handleUnreadMessage(fromId, getFriendStatus(fromId));
    }
}

void MainWindow::handleMessageResponse(const QJsonObject& json)
{
    QString sender = json["sender"].toString();
    QString recipient = json["recipient"].toString();
    int senderId = json["senderId"].toInt();
    int recipientId = json["recipientId"].toInt();

    int chatWindowId = (sender == currentUsername) ? recipientId : senderId;
    processChatMessage(json, chatWindowId);
}

void MainWindow::handleNewMessage(const QJsonObject& json)
{
    int fromId = json["from"].toInt();

    bool shouldShowNotification = !chatWindows.contains(fromId) ||
                                  (chatWindows.contains(fromId) && !chatWindows[fromId]->isVisible());

    if (shouldShowNotification) {
        handleUnreadMessage(fromId, getFriendStatus(fromId));
        if (chatWindows.contains(fromId)) {
            chatWindows[fromId]->processMessage(json);
        }
    } else {
        LOG_DEBUG(QString("Forwarding message to open chat window for user ID: %1").arg(fromId));
        chatWindows[fromId]->processMessage(json);
    }
}

void MainWindow::handleRemoveFriendResponse(const QJsonObject& json)
{
    if (json["status"].toString() == "success") {
        LOG_INFO("Friend removed successfully");
        QMessageBox::information(this, "Success", "Friend removed successfully");
        refreshInvitationsDialog();
    } else {
        LOG_WARNING("Failed to remove friend");
        QMessageBox::warning(this, "Error", "Failed to remove friend");
    }
}

void MainWindow::handleFriendRemoved(const QJsonObject& json)
{
    int friendId = json["friend_id"].toInt();
    closeChatWindow(friendId);

    QMessageBox::information(this, "Friend Removed",
                             "You have been removed from a friend's contact list.");
    refreshInvitationsDialog();
}

void MainWindow::handleFriendsListUpdate(const QJsonObject& json)
{
    updateFriendsList(json["friends"].toArray());
}

// Chat window management methods
void MainWindow::processChatMessage(const QJsonObject& json, int chatWindowId)
{
    if (chatWindows.contains(chatWindowId)) {
        chatWindows[chatWindowId]->processMessage(json);
    } else {
        QString sender = json["sender"].toString();
        if (json["recipient"].toString() == currentUsername) {
            ChatWindow* chatWindow = new ChatWindow(sender, chatWindowId);
            chatWindows[chatWindowId] = chatWindow;

            connect(chatWindow, &ChatWindow::destroyed, [this, chatWindowId]() {
                chatWindows.remove(chatWindowId);
            });

            chatWindow->processMessage(json);
            chatWindow->show();
            chatWindow->activateWindow();
        }
    }
}

void MainWindow::handleUnreadMessage(int fromId, const QString& status)
{
    unreadMessagesMap[fromId] = true;
    updateIconForUser(fromId, status, true);
}

void MainWindow::openChatWindow(QListWidgetItem* item)
{
    if (!item) return;

    int friendId = item->data(Qt::UserRole).toInt();
    QString friendName = item->text();

    if (chatWindows.contains(friendId)) {
        chatWindows[friendId]->show();
        chatWindows[friendId]->activateWindow();

        if (unreadMessagesMap[friendId]) {
            unreadMessagesMap[friendId] = false;
            QString status = getFriendStatus(friendId);
            updateIconForUser(friendId, status, false);

            QJsonObject readMessage = Protocol::MessageStructure::createMessageRead(friendId);
            networkManager.sendMessage(readMessage);
        }
        return;
    }

    ChatWindow* chatWindow = new ChatWindow(friendName, friendId);
    chatWindows[friendId] = chatWindow;

    if (unreadMessagesMap[friendId]) {
        unreadMessagesMap[friendId] = false;
        QString status = getFriendStatus(friendId);
        updateIconForUser(friendId, status, false);

        QJsonObject readMessage = Protocol::MessageStructure::createMessageRead(friendId);
        networkManager.sendMessage(readMessage);
    }

    connect(chatWindow, &QObject::destroyed, this, [this, friendId]() {
        chatWindows.remove(friendId);
        onChatWindowClosed(friendId);
    });

    chatWindow->show();
    chatWindow->activateWindow();
}

void MainWindow::closeChatWindow(int friendId)
{
    if (chatWindows.contains(friendId)) {
        ChatWindow* chatWindow = chatWindows[friendId];
        if (chatWindow) {
            chatWindow->close();
        }
    }
}

// Friends list management methods
void MainWindow::updateFriendsList(const QJsonArray& friends)
{
    LOG_INFO(QString("Updating friends list with %1 friends").arg(friends.size()));
    ui->friendsList->clear();

    QList<QListWidgetItem*> onlineFriends;
    QList<QListWidgetItem*> awayFriends;
    QList<QListWidgetItem*> busyFriends;
    QList<QListWidgetItem*> offlineFriends;

    for (const QJsonValue &friendValue : friends) {
        QJsonObject friendObj = friendValue.toObject();
        QString friendName = friendObj["username"].toString();
        QString friendStatus = friendObj["status"].toString();
        int friendId = friendObj["id"].toInt();

        bool hasUnreadMessages = unreadMessagesMap.value(friendId, false);

        QListWidgetItem* item = new QListWidgetItem();
        item->setText(friendName);
        item->setData(Qt::UserRole, friendId);
        item->setData(Qt::UserRole + 1, friendStatus);
        item->setIcon(getStatusIcon(friendStatus, hasUnreadMessages));
        item->setForeground(Qt::black);

        if (friendStatus == Protocol::UserStatus::ONLINE)
            onlineFriends.append(item);
        else if (friendStatus == Protocol::UserStatus::AWAY)
            awayFriends.append(item);
        else if (friendStatus == Protocol::UserStatus::BUSY)
            busyFriends.append(item);
        else
            offlineFriends.append(item);
    }

    for (auto item : onlineFriends) ui->friendsList->addItem(item);
    for (auto item : awayFriends) ui->friendsList->addItem(item);
    for (auto item : busyFriends) ui->friendsList->addItem(item);
    for (auto item : offlineFriends) ui->friendsList->addItem(item);
}

void MainWindow::updateFriendStatus(int friendId, const QString& status)
{
    for(int i = 0; i < ui->friendsList->count(); ++i) {
        QListWidgetItem* item = ui->friendsList->item(i);
        if(item->data(Qt::UserRole).toInt() == friendId) {
            bool hasUnreadMessages = checkUnreadMessages(friendId);
            item->setIcon(getStatusIcon(status, hasUnreadMessages));
            break;
        }
    }
}

QString MainWindow::getFriendStatus(int friendId) const
{
    for(int i = 0; i < ui->friendsList->count(); ++i) {
        QListWidgetItem* item = ui->friendsList->item(i);
        if(item->data(Qt::UserRole).toInt() == friendId) {
            return item->data(Qt::UserRole + 1).toString();
        }
    }
    return Protocol::UserStatus::OFFLINE;
}

void MainWindow::removeFriend(QListWidgetItem* item)
{
    int friendId = item->data(Qt::UserRole).toInt();
    QString friendName = item->text();

    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Remove Friend",
                                                              QString("Are you sure you want to remove %1 from your friends list?").arg(friendName),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        closeChatWindow(friendId);
        QJsonObject request = Protocol::MessageStructure::createRemoveFriendRequest(friendId);
        networkManager.sendMessage(request);
    }
}

// UI update methods
void MainWindow::updateConnectionStatus(const QString& status)
{
    ui->statusBar->showMessage(status);
}

void MainWindow::updateIconForUser(int userId, const QString& status, bool hasUnread)
{
    for(int i = 0; i < ui->friendsList->count(); ++i) {
        QListWidgetItem* item = ui->friendsList->item(i);
        if(item->data(Qt::UserRole).toInt() == userId) {
            item->setIcon(getStatusIcon(status, hasUnread));
            break;
        }
    }
}

void MainWindow::refreshInvitationsDialog()
{
    if (invitationsDialog && invitationsDialog->isVisible()) {
        invitationsDialog->refreshInvitations();
    }
}

// Event handlers
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

void MainWindow::onMenuSettingsTriggered()
{
    QMessageBox::information(this, "Settings", SETTINGS_DIALOG_MESSAGE);
}

void MainWindow::onMenuExitTriggered()
{
    sendLogoutRequest();
    QApplication::quit();
}

void MainWindow::onMenuAboutTriggered()
{
    QString aboutText = QString(
                            "Jupiter Client v1.0.0\n\n"
                            "Created by: Piotr Lewicki\n"
                            "Build date: %1 %2\n\n"
                            "A modern chat client for communication."
                            ).arg(__DATE__).arg(__TIME__);

    QMessageBox::about(this, "About Jupiter Client", aboutText);
}

void MainWindow::onNetworkError(const QString& error)
{
    LOG_ERROR(QString("Network error: %1").arg(error));
    updateConnectionStatus("Error: " + error);
}

void MainWindow::onDisconnected()
{
    LOG_WARNING("Disconnected from server");
    updateConnectionStatus("Disconnected from server");
    ui->friendsList->clear();
}

void MainWindow::onChatWindowClosed(int friendId)
{
    LOG_DEBUG(QString("Chat window closed for friend ID: %1").arg(friendId));
    chatWindows.remove(friendId);
    unreadMessagesMap[friendId] = false;
}

void MainWindow::onConnectionStatusChanged(const QString& status)
{
    if (networkManager.isConnected() && networkManager.isAuthenticated()) {
        updateConnectionStatus("Connected");
    } else {
        updateConnectionStatus(status);
    }
}

void MainWindow::onMessageReceived(const QJsonObject& json)
{
    QString type = json["type"].toString();

    if (type == Protocol::MessageType::SEARCH_USERS_RESPONSE) {
        handleSearchResponse(json);
    }
    else if (type == Protocol::MessageType::FRIEND_REQUEST_RECEIVED) {
        handleFriendRequest(json);
    }
    else if (type == Protocol::MessageType::FRIEND_REQUEST_ACCEPT_RESPONSE) {
        handleFriendRequestAcceptResponse(json);
    }
    else if (type == Protocol::MessageType::FRIEND_REQUEST_REJECT_RESPONSE) {
        handleFriendRequestRejectResponse(json);
    }
    else if (type == Protocol::MessageType::UNREAD_FROM) {
        handleUnreadMessages(json);
    }
    else if (type == Protocol::MessageType::LOGIN_RESPONSE) {
        handleLoginResponse(json);
    }
    else if (type == Protocol::MessageType::LATEST_MESSAGES_RESPONSE) {
        handleLatestMessages(json);
    }
    else if (type == Protocol::MessageType::MESSAGE_RESPONSE) {
        handleMessageResponse(json);
    }
    else if (type == Protocol::MessageType::NEW_MESSAGES) {
        handleNewMessage(json);
    }
    else if (type == Protocol::MessageType::REMOVE_FRIEND_RESPONSE) {
        handleRemoveFriendResponse(json);
    }
    else if (type == Protocol::MessageType::FRIEND_REMOVED) {
        handleFriendRemoved(json);
    }
    else if (type == Protocol::MessageType::FRIENDS_LIST_RESPONSE ||
             type == Protocol::MessageType::FRIENDS_STATUS_UPDATE) {
        handleFriendsListUpdate(json);
    }
}

void MainWindow::onMenuSearchTriggered()
{
    if (!searchDialog) {
        searchDialog = new SearchDialog(networkManager, this);
        connect(searchDialog, &SearchDialog::friendRequestSent, [this]() {
            refreshInvitationsDialog();
        });
    }
    searchDialog->show();
    searchDialog->raise();
    searchDialog->activateWindow();
}

void MainWindow::showFriendsContextMenu(const QPoint& pos)
{
    QListWidgetItem* item = ui->friendsList->itemAt(pos);
    if (!item) return;

    QMenu contextMenu(this);
    QAction* removeFriendAction = contextMenu.addAction("Remove Friend");
    QAction* selectedAction = contextMenu.exec(ui->friendsList->mapToGlobal(pos));

    if (selectedAction == removeFriendAction) {
        removeFriend(item);
    }
}

// Helper methods
QIcon MainWindow::getStatusIcon(const QString& status, bool hasUnreadMessages) const
{
    QString iconPath = ":/resources/icons/status_";

    if (status == Protocol::UserStatus::ONLINE)
        iconPath += "online";
    else if (status == Protocol::UserStatus::AWAY)
        iconPath += "away";
    else if (status == Protocol::UserStatus::BUSY)
        iconPath += "busy";
    else
        iconPath += "offline";

    if (hasUnreadMessages)
        iconPath += "_msg";

    iconPath += ".svg";
    return QIcon(iconPath);
}

bool MainWindow::checkUnreadMessages(int friendId) const
{
    return unreadMessagesMap.value(friendId, false);
}

void MainWindow::sendLogoutRequest()
{
    if (networkManager.isConnected()) {
        QJsonObject logoutRequest = Protocol::MessageStructure::createLogoutRequest();
        networkManager.sendMessage(logoutRequest);
    }
}

// Setup methods
void MainWindow::setupInvitationsMenu()
{
    QMenu* helpMenu = menuBar()->findChild<QMenu*>("menuHelp");
    if (helpMenu) {
        menuBar()->removeAction(helpMenu->menuAction());
    }

    QMenu* invitationsMenu = menuBar()->addMenu("Invitations");
    QAction* showInvitationsAction = invitationsMenu->addAction("Show Invitations");
    connect(showInvitationsAction, &QAction::triggered,
            this, &MainWindow::onInvitationsActionTriggered);

    if (helpMenu) {
        menuBar()->addMenu(helpMenu);
    }
}

void MainWindow::onInvitationsActionTriggered()
{
    if (!invitationsDialog) {
        invitationsDialog = new InvitationsDialog(networkManager, this);
        connect(&networkManager, &NetworkManager::messageReceived,
                invitationsDialog, &InvitationsDialog::onMessageReceived);
    }
    invitationsDialog->show();
    invitationsDialog->refreshInvitations();
}

bool MainWindow::isFriend(int userId) const
{
    LOG_DEBUG(QString("Checking if user ID %1 is in friends list").arg(userId));

    for(int i = 0; i < ui->friendsList->count(); ++i) {
        QListWidgetItem* item = ui->friendsList->item(i);
        int itemUserId = item->data(Qt::UserRole).toInt();
        LOG_DEBUG(QString("Comparing with friend ID: %1").arg(itemUserId));

        if(itemUserId == userId) {
            LOG_DEBUG(QString("Found user ID %1 in friends list").arg(userId));
            return true;
        }
    }

    LOG_DEBUG(QString("User ID %1 not found in friends list").arg(userId));
    return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    for (auto chatWindow : chatWindows) {
        if (chatWindow) {
            chatWindow->close();
        }
    }
    QMainWindow::closeEvent(event);
}
