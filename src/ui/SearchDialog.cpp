/**
 * @file SearchDialog.cpp
 * @brief Search dialog class implementation
 * @author piotrek-pl
 * @date 2025-01-26 23:19:08
 */

#include "SearchDialog.h"
#include "ui_SearchDialog.h"
#include "MainWindow.h"
#include <QMenu>
#include <QMessageBox>
#include "network/Protocol.h"
#include "utils/Logger.h"

SearchDialog::SearchDialog(NetworkManager& networkManager, MainWindow* parent)
    : QDialog(parent)
    , ui(new Ui::SearchDialog)
    , networkManager(networkManager)
    , mainWindow(parent)
{
    ui->setupUi(this);

    searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(500);
    connect(searchTimer, &QTimer::timeout, this, &SearchDialog::performSearch);

    connect(&networkManager, &NetworkManager::messageReceived,
            this, &SearchDialog::handleServerResponse);

    ui->resultsList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->searchEdit, &QLineEdit::textChanged,
            this, &SearchDialog::onSearchTextChanged);
    connect(ui->resultsList, &QWidget::customContextMenuRequested,
            this, &SearchDialog::showContextMenu);

    LOG_INFO("Search dialog initialized");
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::updatePendingInvitations(const QSet<int>& userIds)
{
    LOG_DEBUG(QString("Updating pending invitations list with %1 users").arg(userIds.size()));
    pendingInvitations = userIds;
    // Odświeżamy widok, aby zaktualizować stan przycisków
    if (!lastSearchResponse.isEmpty()) {
        onSearchResponse(lastSearchResponse);
    }
}

void SearchDialog::onInvitationStatusChanged(int userId)
{
    LOG_DEBUG(QString("Invitation status changed for user ID: %1").arg(userId));
    pendingInvitations.remove(userId);
    // Odświeżamy widok, aby zaktualizować stan przycisków
    if (!lastSearchResponse.isEmpty()) {
        onSearchResponse(lastSearchResponse);
    }
}

void SearchDialog::onSearchResponse(const QJsonObject& response)
{
    lastSearchResponse = response;
    ui->resultsList->clear();
    QJsonArray users = response["users"].toArray();

    for (const QJsonValue& userVal : users) {
        QJsonObject user = userVal.toObject();
        int userId = user["id"].toString().toInt();
        QString username = user["username"].toString();

        QListWidgetItem* item = new QListWidgetItem(username);
        item->setData(Qt::UserRole, userId);

        if (mainWindow && mainWindow->isFriend(userId)) {
            item->setForeground(Qt::gray);
            item->setToolTip("Already in your friends list");
            LOG_DEBUG(QString("User %1 (ID: %2) is marked as friend").arg(username).arg(userId));
        }
        else if (pendingInvitations.contains(userId)) {
            item->setForeground(Qt::blue);
            item->setToolTip("Invitation pending");
            LOG_DEBUG(QString("User %1 (ID: %2) has pending invitation").arg(username).arg(userId));
        }

        ui->resultsList->addItem(item);
    }

    LOG_INFO(QString("Received search results: %1 users found").arg(users.size()));
}

void SearchDialog::showContextMenu(const QPoint& pos)
{
    QListWidgetItem* item = ui->resultsList->itemAt(pos);
    if (!item) return;

    int userId = item->data(Qt::UserRole).toInt();
    QString username = item->text();

    if (!mainWindow) {
        LOG_WARNING("MainWindow pointer is null in SearchDialog");
        return;
    }

    LOG_DEBUG(QString("Showing context menu for user %1 (ID: %2)").arg(username).arg(userId));

    QMenu contextMenu(this);

    if (mainWindow->isFriend(userId)) {
        LOG_DEBUG(QString("User %1 is already a friend").arg(username));
        QAction* alreadyFriendAction = contextMenu.addAction("Already Friends");
        alreadyFriendAction->setEnabled(false);
    }
    else if (pendingInvitations.contains(userId)) {
        LOG_DEBUG(QString("User %1 has pending invitation").arg(username));
        QAction* pendingAction = contextMenu.addAction("Invitation Pending");
        pendingAction->setEnabled(false);
        pendingAction->setToolTip("Wait for response or cancel in Invitations dialog");
    }
    else {
        LOG_DEBUG(QString("Adding invite option for user %1").arg(username));
        QAction* addFriendAction = contextMenu.addAction("Invite");
        connect(addFriendAction, &QAction::triggered, this, [=]() {
            this->sendFriendRequest(userId, username);
        });
    }

    contextMenu.exec(ui->resultsList->mapToGlobal(pos));
}

void SearchDialog::handleServerResponse(const QJsonObject& response)
{
    QString type = response["type"].toString();

    if (type == Protocol::MessageType::ADD_FRIEND_RESPONSE) {
        bool success = response["status"].toString() == "success";
        QString message = response["message"].toString();

        if (success) {
            QMessageBox::information(this, "Success",
                                     "Friend request sent successfully!");
            LOG_INFO("Friend request sent successfully");
            emit friendRequestSent();
        } else {
            QMessageBox::warning(this, "Error",
                                 "Failed to send friend request. " + message);
            LOG_WARNING(QString("Failed to send friend request: %1").arg(message));
        }
    }
    else if (type == Protocol::MessageType::INVITATION_ALREADY_EXISTS) {
        int userId = response["user_id"].toInt();
        QString username = response["username"].toString();
        QString message = response["message"].toString();

        QMessageBox::warning(this, "Warning",
                             "You have already sent an invitation to this user.");
        LOG_WARNING(QString("Attempted to send duplicate invitation to user %1 (ID: %2)")
                        .arg(username).arg(userId));

        // Upewniamy się, że użytkownik jest w liście oczekujących zaproszeń
        pendingInvitations.insert(userId);

        // Odświeżamy widok
        if (!lastSearchResponse.isEmpty()) {
            onSearchResponse(lastSearchResponse);
        }
    }
}

void SearchDialog::onSearchTextChanged(const QString& text)
{
    if (text.length() >= 3) {
        searchTimer->start();
        LOG_DEBUG(QString("Search timer started for query: %1").arg(text));
    } else {
        ui->resultsList->clear();
    }
}

void SearchDialog::performSearch()
{
    QString query = ui->searchEdit->text();
    if (query.length() >= 3) {
        QJsonObject searchRequest = Protocol::MessageStructure::createSearchUsersRequest(query);
        networkManager.sendMessage(searchRequest);
        LOG_DEBUG(QString("Sending search request for query: %1").arg(query));
    }
}

void SearchDialog::sendFriendRequest(int userId, const QString& username)
{
    LOG_INFO(QString("Sending friend request to user %1 (ID: %2)").arg(username).arg(userId));
    QJsonObject request = Protocol::MessageStructure::createAddFriendRequest(userId);
    networkManager.sendMessage(request);
}
