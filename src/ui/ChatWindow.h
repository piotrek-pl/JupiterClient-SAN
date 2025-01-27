// ChatWindow.h

#pragma once

#include <QWidget>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <functional>
#include <unordered_map>
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

signals:
    void messagesRead(int friendId);

private:
    // UI initialization
    void initializeUI();

    // Message handling refactored
    void setupMessageHandlers();
    void dispatchMessage(const QJsonObject& json);

    // Handlers for specific message types
    void handleHistoryResponse(const QJsonObject& json);
    void handleMessageResponse(const QJsonObject& json);
    void handleNewMessages(const QJsonObject& json);

    // Message display
    QString createMessageHtml(const QString& sender, const QString& content,
                              const QDateTime& timestamp, bool isOwn);
    void insertMessageToChat(const QString& messageHtml, bool atBeginning);
    void addMessageToChat(const QString& sender, const QString& content,
                          const QDateTime& timestamp, bool isOwn, bool atEnd = true);

    // Scroll handling
    void updateScrollPosition(int oldScrollPos, int oldMax, bool scrollToEnd);
    bool shouldLoadMoreHistory(int scrollValue) const;

    // History management
    void loadInitialHistory();
    void markMessagesAsRead();

    // UI components
    Ui::ChatWindow *ui;
    NetworkManager& networkManager;

    // Chat properties
    QString friendName;
    int friendId;
    int currentOffset;
    bool hasMoreMessages;
    bool isLoadingHistory;
    bool messagesMarkedAsRead;

    // Dispatch map for message handling
    std::unordered_map<QString, std::function<void(const QJsonObject&)>> messageHandlers;

protected:
    void showEvent(QShowEvent* event) override;
};
