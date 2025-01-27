/**
 * @file ChatWindow.cpp
 * @brief Chat window class implementation
 * @author piotrek-pl
 * @date 2025-01-27 08:34:02
 */

#include "ChatWindow.h"
#include "ui_ChatWindow.h"
#include <QScrollBar>
#include "network/Protocol.h"

ChatWindow::ChatWindow(const QString& friendName, int friendId, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWindow)
    , networkManager(NetworkManager::getInstance())
    , friendName(friendName)
    , friendId(friendId)
    , currentOffset(0)
    , hasMoreMessages(true)
    , isLoadingHistory(false)
    , messagesMarkedAsRead(false)
{
    ui->setupUi(this);
    setWindowTitle("Chat with " + friendName);
    initializeUI();
    loadInitialHistory();

    connect(&networkManager, &NetworkManager::messageReceived,
            this, &ChatWindow::onMessageReceived);
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::initializeUI()
{
    connect(ui->sendButton, &QPushButton::clicked,
            this, &ChatWindow::onSendMessageClicked);
    connect(ui->messageLineEdit, &QLineEdit::returnPressed,
            this, &ChatWindow::onSendMessageClicked);
    connect(ui->chatTextEdit->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &ChatWindow::onScrollValueChanged);

    ui->chatTextEdit->setReadOnly(true);
}

bool ChatWindow::shouldLoadMoreHistory(int scrollValue) const
{
    QScrollBar* scrollBar = ui->chatTextEdit->verticalScrollBar();
    return scrollValue <= scrollBar->maximum() * 0.1;
}

void ChatWindow::onScrollValueChanged(int value)
{
    if (shouldLoadMoreHistory(value)) {
        loadMoreHistory();
    }
}

void ChatWindow::loadInitialHistory()
{
    QJsonObject request;
    request["type"] = Protocol::MessageType::GET_LATEST_MESSAGES;
    request["friend_id"] = friendId;
    request["limit"] = Protocol::ChatHistory::MESSAGE_BATCH_SIZE;

    networkManager.sendMessage(request);
    isLoadingHistory = true;
}

void ChatWindow::loadMoreHistory()
{
    if (!hasMoreMessages || isLoadingHistory) return;

    QJsonObject request;
    request["type"] = Protocol::MessageType::GET_MORE_HISTORY;
    request["friend_id"] = friendId;
    request["offset"] = currentOffset;

    networkManager.sendMessage(request);
    isLoadingHistory = true;
}

QString ChatWindow::createMessageHtml(const QString& sender, const QString& content,
                                      const QDateTime& timestamp, bool isOwn)
{
    QString timeStr = timestamp.toString("HH:mm:ss");
    QString alignment = isOwn ? "right" : "left";

    return QString("<div style='text-align: %1;'><b>%2</b> [%3]<br>%4</div>")
        .arg(alignment)
        .arg(sender)
        .arg(timeStr)
        .arg(content);
}

void ChatWindow::insertMessageToChat(const QString& messageHtml, bool atBeginning)
{
    QTextCursor cursor(ui->chatTextEdit->document());

    if (atBeginning) {
        cursor.movePosition(QTextCursor::Start);
    } else {
        cursor.movePosition(QTextCursor::End);
    }

    cursor.insertText("\n");
    cursor.insertHtml(messageHtml);
}

void ChatWindow::updateScrollPosition(int oldScrollPos, int oldMax, bool scrollToEnd)
{
    QScrollBar* scrollBar = ui->chatTextEdit->verticalScrollBar();
    if (scrollToEnd) {
        scrollBar->setValue(scrollBar->maximum());
    } else {
        int newMax = scrollBar->maximum();
        scrollBar->setValue(oldScrollPos + (newMax - oldMax));
    }
}

void ChatWindow::processHistoryMessages(const QJsonArray& messages, bool isOlderMessages)
{
    QString newMessages;
    for (const QJsonValue& msgVal : messages) {
        QJsonObject msg = msgVal.toObject();
        QString sender = msg["sender"].toString();
        QString content = msg["content"].toString();
        QDateTime timestamp = QDateTime::fromString(msg["timestamp"].toString(), Qt::ISODate);
        bool isOwn = (sender != friendName);

        QString messageHtml = createMessageHtml(sender, content, timestamp, isOwn);

        if (isOlderMessages) {
            newMessages = messageHtml + newMessages;
        } else {
            newMessages += messageHtml;
        }
    }

    insertMessageToChat(newMessages, isOlderMessages);
    currentOffset += messages.size();
}

void ChatWindow::handleChatHistoryResponse(const QJsonObject& json, const QString& type)
{
    QJsonArray messages = json["messages"].toArray();
    int oldScrollPos = ui->chatTextEdit->verticalScrollBar()->value();
    int oldMax = ui->chatTextEdit->verticalScrollBar()->maximum();

    bool isOlderMessages = (type == Protocol::MessageType::MORE_HISTORY_RESPONSE);
    processHistoryMessages(messages, isOlderMessages);

    hasMoreMessages = json["has_more"].toBool();
    isLoadingHistory = false;

    updateScrollPosition(oldScrollPos, oldMax, !isOlderMessages);

    if (type == Protocol::MessageType::LATEST_MESSAGES_RESPONSE && isVisible() && !messagesMarkedAsRead) {
        markMessagesAsRead();
    }
}

void ChatWindow::handleMessageResponse(const QJsonObject& json)
{
    QString sender = json["sender"].toString();
    QString content = json["content"].toString();
    QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(json["timestamp"].toInteger());

    if (sender == friendName || json["recipient"].toString() == friendName) {
        bool isOwn = (sender != friendName);
        addMessageToChat(sender, content, timestamp, isOwn, true);
    }
}

void ChatWindow::handleNewMessage(const QJsonObject& json)
{
    QString content = json["content"].toString();
    int fromId = json["from"].toInt();
    QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(json["timestamp"].toDouble()));

    LOG_INFO(QString("Processing new message from user %1: %2").arg(fromId).arg(content));

    if (fromId == friendId) {
        LOG_INFO("Message is from friend, adding to chat");
        addMessageToChat(friendName, content, timestamp, false, true);

        if (isVisible()) {
            if (!messagesMarkedAsRead) {
                markMessagesAsRead();
            }
        } else {
            messagesMarkedAsRead = false;
        }
    }
}

void ChatWindow::onMessageReceived(const QJsonObject& json)
{
    QString type = json["type"].toString();
    LOG_INFO(QString("ChatWindow::onMessageReceived - Received message type: %1").arg(type));

    if (type == Protocol::MessageType::LATEST_MESSAGES_RESPONSE ||
        type == Protocol::MessageType::CHAT_HISTORY_RESPONSE ||
        type == Protocol::MessageType::MORE_HISTORY_RESPONSE)
    {
        handleChatHistoryResponse(json, type);
    }
    else if (type == Protocol::MessageType::MESSAGE_RESPONSE)
    {
        handleMessageResponse(json);
    }
    else if (type == Protocol::MessageType::NEW_MESSAGES)
    {
        handleNewMessage(json);
    }
}

void ChatWindow::markMessagesAsRead()
{
    QJsonObject readNotification = Protocol::MessageStructure::createMessageRead(friendId);
    networkManager.sendMessage(readNotification);
    messagesMarkedAsRead = true;
    emit messagesRead(friendId);
}

void ChatWindow::onSendMessageClicked()
{
    QString message = ui->messageLineEdit->text().trimmed();
    if (message.isEmpty()) return;

    QJsonObject messageRequest = Protocol::MessageStructure::createMessage(friendId, message);
    QDateTime currentTime = QDateTime::currentDateTime();
    QString sender = networkManager.getUsername();

    addMessageToChat(sender, message, currentTime, true, true);
    networkManager.sendMessage(messageRequest);
    ui->messageLineEdit->clear();
}

void ChatWindow::addMessageToChat(const QString& sender, const QString& content,
                                  const QDateTime& timestamp, bool isOwn, bool atEnd)
{
    QString messageHtml = createMessageHtml(sender, content, timestamp, isOwn);
    insertMessageToChat(messageHtml, false);

    if (atEnd) {
        ui->chatTextEdit->verticalScrollBar()->setValue(
            ui->chatTextEdit->verticalScrollBar()->maximum());
    }
}

void ChatWindow::processMessage(const QJsonObject& message)
{
    onMessageReceived(message);
}

void ChatWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (!messagesMarkedAsRead) {
        markMessagesAsRead();
    }
}
