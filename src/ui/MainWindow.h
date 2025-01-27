/**
 * @file MainWindow.h
 * @brief Main window class definition
 * @author piotrek-pl
 * @date 2025-01-27 08:58:55
 */

#pragma once

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QJsonArray>
#include <QMap>
#include <qlistwidget.h>
#include "network/NetworkManager.h"
#include "config/ConfigManager.h"
#include "ChatWindow.h"
#include "InvitationsDialog.h"

class SearchDialog;  // Forward declaration

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool isFriend(int userId) const;

public slots:
    void setCurrentUsername(const QString& username) {
        currentUsername = username;
    }

private slots:
    // UI event handlers
    void onStatusChanged(int index);
    void onRefreshFriendsListClicked();
    void onMenuSettingsTriggered();
    void onMenuExitTriggered();
    void onMenuAboutTriggered();
    void onMenuSearchTriggered();
    void showFriendsContextMenu(const QPoint& pos);
    void onInvitationsActionTriggered();

    // Network event handlers
    void onConnectionStatusChanged(const QString& status);
    void onMessageReceived(const QJsonObject& message);
    void onNetworkError(const QString& error);
    void onDisconnected();
    void onChatWindowClosed(int friendId);

private:
    // Initialization
    void initializeUI();
    void setupNetworkConnections();
    void setupInvitationsMenu();
    void setupStatusComboBox();
    void setupUIConnections();
    void setupFriendsList();

    // Message handlers
    void handleSearchResponse(const QJsonObject& json);
    void handleFriendRequest(const QJsonObject& json);
    void handleFriendRequestAcceptResponse(const QJsonObject& json);
    void handleFriendRequestRejectResponse(const QJsonObject& json);
    void handleUnreadMessages(const QJsonObject& json);
    void handleLoginResponse(const QJsonObject& json);
    void handleLatestMessages(const QJsonObject& json);
    void handleMessageResponse(const QJsonObject& json);
    void handleNewMessage(const QJsonObject& json);
    void handleRemoveFriendResponse(const QJsonObject& json);
    void handleFriendRemoved(const QJsonObject& json);
    void handleFriendsListUpdate(const QJsonObject& json);

    // Chat window management
    void openChatWindow(QListWidgetItem* item);
    void closeChatWindow(int friendId);
    void processChatMessage(const QJsonObject& json, int chatWindowId);
    void handleUnreadMessage(int fromId, const QString& status);

    // Friends list management
    void updateFriendsList(const QJsonArray& friends);
    void sortFriendsListByStatus();
    void updateFriendStatus(int friendId, const QString& status);
    QString getFriendStatus(int friendId) const;
    void removeFriend(QListWidgetItem* item);

    // UI updates
    void updateConnectionStatus(const QString& status);
    void updateIconForUser(int userId, const QString& status, bool hasUnread);
    void refreshInvitationsDialog();

    // Helper methods
    QIcon getStatusIcon(const QString& status, bool hasUnreadMessages) const;
    bool checkUnreadMessages(int friendId) const;
    void sendLogoutRequest();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    NetworkManager& networkManager;
    QString currentUsername;
    QString currentStatus;
    ConfigManager::ConnectionConfig connectionConfig;

    QMap<int, ChatWindow*> chatWindows;
    QMap<int, bool> unreadMessagesMap;
    SearchDialog* searchDialog;
    InvitationsDialog* invitationsDialog;

    // Constants for UI configuration
    static const QString DEFAULT_WINDOW_TITLE;
    static const QString MESSAGE_INPUT_PLACEHOLDER;
    static const QString SETTINGS_DIALOG_MESSAGE;
};

// Static const definitions
inline const QString MainWindow::DEFAULT_WINDOW_TITLE = "Jupiter Client";
inline const QString MainWindow::MESSAGE_INPUT_PLACEHOLDER = "Type your message here...";
inline const QString MainWindow::SETTINGS_DIALOG_MESSAGE = "Settings functionality will be implemented in future versions.";
