/**
 * @file ChatWindow.h
 * @brief Chat window class definition
 * @author piotrek-pl
 * @date 2025-01-27 08:34:02
 */

#pragma once

#include <QWidget>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
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

    // Message handling
    void handleChatHistoryResponse(const QJsonObject& json, const QString& type);
    void handleMessageResponse(const QJsonObject& json);
    void handleNewMessage(const QJsonObject& json);
    void processHistoryMessages(const QJsonArray& messages, bool isOlderMessages);

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

protected:
    void showEvent(QShowEvent* event) override;
};
