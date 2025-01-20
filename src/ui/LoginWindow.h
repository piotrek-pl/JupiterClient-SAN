/**
 * @file LoginWindow.h
 * @brief Login window class definition
 * @author piotrek-pl
 * @date 2025-01-20 14:01:42
 */

#pragma once

#include <QWidget>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonObject>
#include <QDateTime>
#include "config/ConfigManager.h"
#include "utils/Logger.h"
#include "network/Protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

    QTcpSocket* getSocket() { return &socket; }

signals:
    void loginSuccessful();
    void registrationSuccessful();

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QTcpSocket::SocketError socketError);
    void checkConnection();

private:
    void initializeUI();
    void initializeNetworking();
    void updateConnectionStatus(const QString& status);
    void sendPong(qint64 timestamp);
    void processIncomingMessage(const QJsonObject& json);
    void scheduleReconnection();

    Ui::LoginWindow *ui;
    QTcpSocket socket;
    QTimer* connectionCheckTimer;
    qint64 lastPongTime;
    int missedPings;
    int reconnectAttempts;
    QString currentUsername;
    QString currentPassword;
    bool isReconnecting;
    bool isAuthenticated;

    ConfigManager::ConnectionConfig connectionConfig;
};
