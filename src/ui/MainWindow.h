/**
 * @file MainWindow.h
 * @brief Main window class definition
 * @author piotrek-pl
 * @date 2025-01-20 14:03:26
 */

#pragma once

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonObject>
#include <QDateTime>
#include "config/ConfigManager.h"
#include "utils/Logger.h"
#include "network/Protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setSocket(QTcpSocket *sharedSocket);

private slots:
    void onReadyRead();
    void onDisconnected();
    void onError(QTcpSocket::SocketError socketError);
    void checkConnection();
    void onSendMessageClicked();
    void onStatusChanged(int index);
    void onRefreshFriendsListClicked();
    void onMenuSettingsTriggered();
    void onMenuExitTriggered();
    void onMenuAboutTriggered();

private:
    void initializeUI();
    void initializeNetworking();
    void updateConnectionStatus(const QString& status);
    void processIncomingMessage(const QJsonObject& json);
    void sendPong(qint64 timestamp);
    void updateFriendsList(const QJsonArray& friends);
    void addMessageToChat(const QString& sender, const QString& content,
                          const QDateTime& timestamp, bool isOwn = false);
    void scheduleReconnection();

    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QTimer* connectionCheckTimer;
    qint64 lastPongTime;
    int missedPings;
    int reconnectAttempts;
    bool isReconnecting;
    bool isAuthenticated;

    QString currentUsername;
    QString currentStatus;
    ConfigManager::ConnectionConfig connectionConfig;
};
