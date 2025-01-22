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
    void onScrollValueChanged(int value);
    void loadMoreHistory();

private:
    void initializeUI();
    void addMessageToChat(const QString& sender, const QString& content,
                          const QDateTime& timestamp, bool isOwn, bool atEnd = true);
    void loadInitialHistory();

    Ui::ChatWindow *ui;
    NetworkManager& networkManager;
    QString friendName;
    int friendId;
    int currentOffset;
    bool hasMoreMessages;
    bool isLoadingHistory;
};
