/**
 * @file MainWindow.h
 * @brief Main window class definition
 * @author piotrek-pl
 * @date 2025-01-21 13:03:48
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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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

    // Network event handlers
    void onConnectionStatusChanged(const QString& status);
    void onMessageReceived(const QJsonObject& message);
    void onNetworkError(const QString& error);
    void onDisconnected();

private:
    void initializeUI();
    void setupNetworkConnections();
    void updateConnectionStatus(const QString& status);
    void updateFriendsList(const QJsonArray& friends);
    void addMessageToChat(const QString& sender, const QString& content,
                          const QDateTime& timestamp, bool isOwn);
    void openChatWindow(QListWidgetItem* item);

    Ui::MainWindow *ui;
    NetworkManager& networkManager;
    QString currentUsername;
    QString currentStatus;
    ConfigManager::ConnectionConfig connectionConfig;

    QMap<int, ChatWindow*> chatWindows;

    // Constants for UI configuration
    static const QString DEFAULT_WINDOW_TITLE;
    static const QString MESSAGE_INPUT_PLACEHOLDER;
    static const QString SETTINGS_DIALOG_MESSAGE;
};

// Static const definitions (można umieścić w cpp)
inline const QString MainWindow::DEFAULT_WINDOW_TITLE = "Jupiter Client";
inline const QString MainWindow::MESSAGE_INPUT_PLACEHOLDER = "Type your message here...";
inline const QString MainWindow::SETTINGS_DIALOG_MESSAGE = "Settings functionality will be implemented in future versions.";
