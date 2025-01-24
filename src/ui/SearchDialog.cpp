/**
 * @file SearchDialog.cpp
 * @brief Search dialog class implementation
 * @author piotrek-pl
 * @date 2025-01-24 12:46:36
 */

#include "SearchDialog.h"
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QMenu>
#include <QMessageBox>
#include "network/Protocol.h"
#include "utils/Logger.h"

SearchDialog::SearchDialog(NetworkManager& networkManager, MainWindow* parent)
    : QDialog(parent)
    , networkManager(networkManager)
    , mainWindow(parent)
{
    setWindowTitle("Search Users");
    setMinimumWidth(300);
    setMinimumHeight(400);

    setupUI();

    searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(500);
    connect(searchTimer, &QTimer::timeout, this, &SearchDialog::performSearch);

    // Dodajemy połączenie do obsługi odpowiedzi na zaproszenie
    connect(&networkManager, &NetworkManager::messageReceived,
            this, &SearchDialog::handleServerResponse);

    LOG_INFO("Search dialog initialized");
}

void SearchDialog::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Enter username to search...");
    layout->addWidget(searchEdit);

    resultsList = new QListWidget(this);
    resultsList->setContextMenuPolicy(Qt::CustomContextMenu);
    layout->addWidget(resultsList);

    connect(searchEdit, &QLineEdit::textChanged,
            this, &SearchDialog::onSearchTextChanged);
    connect(resultsList, &QWidget::customContextMenuRequested,
            this, &SearchDialog::showContextMenu);

    LOG_DEBUG("Search dialog UI setup completed");
}

void SearchDialog::onSearchTextChanged(const QString& text)
{
    if (text.length() >= 3) {
        searchTimer->start();
        LOG_DEBUG(QString("Search timer started for query: %1").arg(text));
    } else {
        resultsList->clear();
    }
}

void SearchDialog::performSearch()
{
    QString query = searchEdit->text();
    if (query.length() >= 3) {
        QJsonObject searchRequest = Protocol::MessageStructure::createSearchUsersRequest(query);
        networkManager.sendMessage(searchRequest);
        LOG_DEBUG(QString("Sending search request for query: %1").arg(query));
    }
}

void SearchDialog::onSearchResponse(const QJsonObject& response)
{
    if (response["type"].toString() != Protocol::MessageType::SEARCH_USERS_RESPONSE) {
        return;
    }

    resultsList->clear();
    QJsonArray users = response["users"].toArray();

    for (const QJsonValue& userVal : users) {
        QJsonObject user = userVal.toObject();
        // Zmiana tutaj - konwersja string na int
        int userId = user["id"].toString().toInt();  // Konwertujemy string na int
        QString username = user["username"].toString();

        QListWidgetItem* item = new QListWidgetItem(username);
        item->setData(Qt::UserRole, userId);

        if (mainWindow && mainWindow->isFriend(userId)) {
            item->setForeground(Qt::gray);
            item->setToolTip("Already in your friends list");
            LOG_DEBUG(QString("User %1 (ID: %2) is marked as friend").arg(username).arg(userId));
        }

        resultsList->addItem(item);
    }

    LOG_INFO(QString("Received search results: %1 users found").arg(users.size()));
}

void SearchDialog::showContextMenu(const QPoint& pos)
{
    QListWidgetItem* item = resultsList->itemAt(pos);
    if (!item) return;

    int userId = item->data(Qt::UserRole).toInt();
    QString username = item->text();

    if (!mainWindow) {
        LOG_WARNING("MainWindow pointer is null in SearchDialog");
        return;
    }

    LOG_DEBUG(QString("Showing context menu for user %1 (ID: %2)").arg(username).arg(userId));
    bool isFriend = mainWindow->isFriend(userId);
    LOG_DEBUG(QString("isFriend check result: %1").arg(isFriend));

    QMenu contextMenu(this);

    if (isFriend) {
        LOG_DEBUG(QString("User %1 is already a friend, showing disabled menu").arg(username));
        QAction* alreadyFriendAction = contextMenu.addAction("Already Friends");
        alreadyFriendAction->setEnabled(false);
        contextMenu.exec(resultsList->mapToGlobal(pos));
        return;
    }

    LOG_DEBUG(QString("User %1 is not a friend, showing add friend menu").arg(username));
    QAction* addFriendAction = contextMenu.addAction("Invite");
    QAction* selectedAction = contextMenu.exec(resultsList->mapToGlobal(pos));

    if (selectedAction && selectedAction == addFriendAction) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  "Add Friend",
                                                                  QString("Do you want to add %1 to your friends list?").arg(username),
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            LOG_INFO(QString("Sending friend request to user %1 (ID: %2)").arg(username).arg(userId));
            QJsonObject request = Protocol::MessageStructure::createAddFriendRequest(userId);
            networkManager.sendMessage(request);
        }
    }
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
        } else {
            QMessageBox::warning(this, "Error",
                                 "Failed to send friend request. " + message);
            LOG_WARNING(QString("Failed to send friend request: %1").arg(message));
        }
    }
    else if (type == Protocol::MessageType::SEARCH_USERS_RESPONSE) {
        onSearchResponse(response);
    }
}
