#pragma once

#include <QWidget>
#include <QDateTime>
#include <QJsonObject>
#include "network/NetworkManager.h"

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget {
    Q_OBJECT

public:
    explicit ChatWindow(const QString& friendName, int friendId, QWidget *parent = nullptr);
    ~ChatWindow();

    void processMessage(const QJsonObject& message);

private slots:
    void onSendMessageClicked();
    void onMessageReceived(const QJsonObject& message);

private:
    void initializeUI();
    void addMessageToChat(const QString& sender, const QString& content,
                          const QDateTime& timestamp, bool isOwn);

    Ui::ChatWindow *ui;
    NetworkManager& networkManager;
    QString friendName;
    int friendId;
};
