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

    ui->friendsList->setIconSize(QSize(16, 16));
    ui->friendsList->setSpacing(2);
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

    if (type == Protocol::MessageType::LOGIN_RESPONSE) {
        if (json["status"].toString() == "success") {
            currentUsername = json["username"].toString();
            if (json.contains("friends")) {
                updateFriendsList(json["friends"].toArray());
            }
        }
    }
    else if (type == Protocol::MessageType::LATEST_MESSAGES_RESPONSE) {
        QJsonArray messages = json["messages"].toArray();
        for (const QJsonValue &messageValue : messages) {
            QJsonObject messageObj = messageValue.toObject();
            int fromId = messageObj["from"].toInt();

            // Oznacz wiadomości jako nieprzeczytane
            unreadMessagesMap[fromId] = true;

            // Zaktualizuj ikonę na liście znajomych
            for(int i = 0; i < ui->friendsList->count(); ++i) {
                QListWidgetItem* item = ui->friendsList->item(i);
                if(item->data(Qt::UserRole).toInt() == fromId) {
                    QString status = item->data(Qt::UserRole + 1).toString();
                    LOG_DEBUG(QString("Updating icon for user ID %1 with status %2 and unread messages")
                                  .arg(fromId)
                                  .arg(status));
                    item->setIcon(getStatusIcon(status, true));
                    break;
                }
            }
        }
    }
    else if (type == Protocol::MessageType::MESSAGE_RESPONSE) {
        QString sender = json["sender"].toString();
        QString recipient = json["recipient"].toString();
        int senderId = json["senderId"].toInt();
        int recipientId = json["recipientId"].toInt();

        // Określ ID okna czatu
        int chatWindowId = (sender == currentUsername) ? recipientId : senderId;

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
    else if (type == Protocol::MessageType::NEW_MESSAGES) {
        int fromId = json["from"].toInt();
        QString content = json["content"].toString();
        qint64 timestamp = json["timestamp"].toInteger();

        LOG_DEBUG(QString("Processing new message from user ID: %1").arg(fromId));

        // Sprawdź, czy okno czatu jest otwarte i widoczne
        bool shouldShowNotification = !chatWindows.contains(fromId) ||
                                      (chatWindows.contains(fromId) && !chatWindows[fromId]->isVisible());

        if (shouldShowNotification) {
            // Oznacz wiadomość jako nieprzeczytaną
            unreadMessagesMap[fromId] = true;

            // Znajdź item na liście znajomych i zaktualizuj jego ikonę
            for(int i = 0; i < ui->friendsList->count(); ++i) {
                QListWidgetItem* item = ui->friendsList->item(i);
                if(item->data(Qt::UserRole).toInt() == fromId) {
                    QString status = item->data(Qt::UserRole + 1).toString();
                    LOG_DEBUG(QString("Updating icon for user ID %1 with status %2 and unread messages")
                                  .arg(fromId)
                                  .arg(status));
                    item->setIcon(getStatusIcon(status, true));
                    break;
                }
            }

            // Jeśli okno istnieje ale jest niewidoczne, dodaj wiadomość do historii
            if (chatWindows.contains(fromId)) {
                chatWindows[fromId]->processMessage(json);
            }
        } else {
            // Jeśli okno jest otwarte i widoczne, przekaż wiadomość do okna czatu
            LOG_DEBUG(QString("Forwarding message to open chat window for user ID: %1").arg(fromId));
            chatWindows[fromId]->processMessage(json);
        }
    }
    else if (type == Protocol::MessageType::FRIENDS_LIST_RESPONSE ||
             type == Protocol::MessageType::FRIENDS_STATUS_UPDATE) {
        updateFriendsList(json["friends"].toArray());
    }
    else {
        LOG_WARNING(QString("Received unknown message type: %1").arg(type));
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
        bool hasUnreadMessages = friendObj.contains("unreadMessages") ?
                                     friendObj["unreadMessages"].toBool() :
                                     unreadMessagesMap.value(friendId, false);

        // Zapisz stan nieprzeczytanych wiadomości
        unreadMessagesMap[friendId] = hasUnreadMessages;

        QListWidgetItem* item = new QListWidgetItem();
        item->setText(friendName);
        item->setData(Qt::UserRole, friendId);
        item->setData(Qt::UserRole + 1, friendStatus);  // Zapisz status
        item->setIcon(getStatusIcon(friendStatus, hasUnreadMessages));
        item->setForeground(Qt::black);

        ui->friendsList->addItem(item);
    }

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
    QString friendName = item->text();

    // Jeśli okno już istnieje
    if (chatWindows.contains(friendId)) {
        chatWindows[friendId]->show();
        chatWindows[friendId]->activateWindow();

        // Dodajemy tę część - resetujemy stan nieprzeczytanych wiadomości
        if (unreadMessagesMap[friendId]) {
            unreadMessagesMap[friendId] = false;
            QString status = getFriendStatus(friendId);
            // Aktualizuj ikonę - usuń kopertę
            for(int i = 0; i < ui->friendsList->count(); ++i) {
                QListWidgetItem* listItem = ui->friendsList->item(i);
                if(listItem->data(Qt::UserRole).toInt() == friendId) {
                    listItem->setIcon(getStatusIcon(status, false));
                    break;
                }
            }
            // Wyślij potwierdzenie przeczytania do serwera
            QJsonObject readMessage = Protocol::MessageStructure::createMessageRead(friendId);
            networkManager.sendMessage(readMessage);
        }
        return;
    }

    // Tworzenie nowego okna
    ChatWindow* chatWindow = new ChatWindow(friendName, friendId);
    chatWindows[friendId] = chatWindow;

    // Jeśli były nieprzeczytane wiadomości, oznacz je jako przeczytane
    if (unreadMessagesMap[friendId]) {
        unreadMessagesMap[friendId] = false;
        QString status = getFriendStatus(friendId);
        for(int i = 0; i < ui->friendsList->count(); ++i) {
            QListWidgetItem* listItem = ui->friendsList->item(i);
            if(listItem->data(Qt::UserRole).toInt() == friendId) {
                listItem->setIcon(getStatusIcon(status, false));
                break;
            }
        }

        QJsonObject readMessage = Protocol::MessageStructure::createMessageRead(friendId);
        networkManager.sendMessage(readMessage);
    }

    // Połącz sygnał destroyed z nowym slotem
    connect(chatWindow, &QObject::destroyed, this, [this, friendId]() {
        chatWindows.remove(friendId);
        onChatWindowClosed(friendId);
    });

    chatWindow->show();
    chatWindow->activateWindow();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Zamknij wszystkie otwarte okna (np. okna czatu)
    for (auto chatWindow : chatWindows) {
        if (chatWindow) {
            chatWindow->close();
        }
    }

    // Możesz dodać inne czynności czyszczące tutaj (zapis ustawień, wylogowanie itp.)
    // a następnie wywołać oryginalną funkcję zamykającą QMainWindow:
    QMainWindow::closeEvent(event);
}

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
        iconPath += "_msg";  // Dodaje suffix "_msg" dla ikon z kopertą

    iconPath += ".svg";
    return QIcon(iconPath);
}

bool MainWindow::checkUnreadMessages(int friendId) const
{
    return unreadMessagesMap.value(friendId, false);
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

void MainWindow::onChatWindowClosed(int friendId)
{
    // Po zamknięciu okna czatu, przygotuj się na nowe wiadomości
    LOG_DEBUG(QString("Chat window closed for friend ID: %1").arg(friendId));
    chatWindows.remove(friendId);

    // Resetujemy stan nieprzeczytanych wiadomości
    unreadMessagesMap[friendId] = false;
}
