// ChatWindow.cpp

#include "ChatWindow.h"
#include "ui_ChatWindow.h"
#include <QScrollBar>
#include <QTextCursor>
#include "network/Protocol.h"
#include "utils/Logger.h" // Assuming a LOG_INFO or similar macro is defined here

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

    // Setup our dispatch map
    setupMessageHandlers();
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

void ChatWindow::setupMessageHandlers()
{
    using namespace Protocol::MessageType;

    // Combine LATEST_MESSAGES_RESPONSE, CHAT_HISTORY_RESPONSE, and MORE_HISTORY_RESPONSE
    // into one handler for clarity:
    messageHandlers[LATEST_MESSAGES_RESPONSE] = [&](const QJsonObject& json) {
        handleHistoryResponse(json);
    };
    messageHandlers[CHAT_HISTORY_RESPONSE] = [&](const QJsonObject& json) {
        handleHistoryResponse(json);
    };
    messageHandlers[MORE_HISTORY_RESPONSE] = [&](const QJsonObject& json) {
        handleHistoryResponse(json);
    };

    messageHandlers[MESSAGE_RESPONSE] = [&](const QJsonObject& json) {
        handleMessageResponse(json);
    };
    messageHandlers[NEW_MESSAGES] = [&](const QJsonObject& json) {
        handleNewMessages(json);
    };
}

void ChatWindow::onMessageReceived(const QJsonObject& json)
{
    QString type = json["type"].toString();

    // Jeśli to nowa wiadomość, obsługuj ją tylko gdy przychodzi bezpośrednio z NetworkManager
    if (type == Protocol::MessageType::NEW_MESSAGES && !sender()) {
        // Ignoruj wiadomości przekazane z MainWindow (gdy sender() == nullptr)
        return;
    }

    LOG_INFO(QString("ChatWindow::onMessageReceived - Received message type: %1").arg(type));
    dispatchMessage(json);
}

void ChatWindow::dispatchMessage(const QJsonObject& json)
{
    QString type = json["type"].toString();
    LOG_INFO(QString("ChatWindow::onMessageReceived - Received message type: %1").arg(type));

    // Look for a matching handler
    auto it = messageHandlers.find(type);
    if (it != messageHandlers.end()) {
        it->second(json);
    } else {
        LOG_INFO("ChatWindow::dispatchMessage - Unknown message type received.");
    }
}

void ChatWindow::handleHistoryResponse(const QJsonObject& json)
{
    QJsonArray messages = json["messages"].toArray();
    int oldScrollPos = ui->chatTextEdit->verticalScrollBar()->value();
    int oldMax = ui->chatTextEdit->verticalScrollBar()->maximum();

    QString type = json["type"].toString();
    QString newMessages;
    for (const QJsonValue& msgVal : messages)
    {
        QJsonObject msg = msgVal.toObject();
        QString sender = msg["sender"].toString();
        QString content = msg["content"].toString();
        QDateTime timestamp = QDateTime::fromString(msg["timestamp"].toString(), Qt::ISODate);
        bool isOwn = (sender != friendName);

        QString timeStr = timestamp.toString("HH:mm:ss");
        QString messageHtml;

        if (isOwn) {
            messageHtml = QString("<div style='text-align: right;'><b>%1</b> [%2]<br>%3</div>")
            .arg(sender)
                .arg(timeStr)
                .arg(content);
        } else {
            messageHtml = QString("<div style='text-align: left;'><b>%1</b> [%2]<br>%3</div>")
            .arg(sender)
                .arg(timeStr)
                .arg(content);
        }

        // If it's a MORE_HISTORY_RESPONSE, prepend older messages
        if (type == Protocol::MessageType::MORE_HISTORY_RESPONSE) {
            newMessages = messageHtml + newMessages;
        } else {
            newMessages += messageHtml;
        }
    }

    // Insert the new messages in the correct position
    QTextCursor cursor(ui->chatTextEdit->document());
    if (type == Protocol::MessageType::MORE_HISTORY_RESPONSE) {
        cursor.movePosition(QTextCursor::Start);
        cursor.insertHtml(newMessages);
        // If not empty, insert a newline
        if (!ui->chatTextEdit->toPlainText().isEmpty()) {
            cursor.movePosition(QTextCursor::Start);
            cursor.insertText("\n");
        }
    } else {
        cursor.movePosition(QTextCursor::End);
        cursor.insertText("\n");
        cursor.insertHtml(newMessages);
    }

    currentOffset += messages.size();
    hasMoreMessages = json["has_more"].toBool();
    isLoadingHistory = false;

    if (type == Protocol::MessageType::MORE_HISTORY_RESPONSE) {
        int newMax = ui->chatTextEdit->verticalScrollBar()->maximum();
        ui->chatTextEdit->verticalScrollBar()->setValue(oldScrollPos + (newMax - oldMax));
    } else {
        ui->chatTextEdit->verticalScrollBar()->setValue(
            ui->chatTextEdit->verticalScrollBar()->maximum());
    }

    // Mark messages as read if they are the latest messages and window is visible
    if (type == Protocol::MessageType::LATEST_MESSAGES_RESPONSE && isVisible() && !messagesMarkedAsRead) {
        QJsonObject readNotification = Protocol::MessageStructure::createMessageRead(friendId);
        networkManager.sendMessage(readNotification);
        messagesMarkedAsRead = true;
        emit messagesRead(friendId);
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

void ChatWindow::handleNewMessages(const QJsonObject& json)
{
    QString content = json["content"].toString();
    int fromId = json["from"].toInt();
    QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(json["timestamp"].toDouble()));

    LOG_INFO(QString("Processing new message from user %1: %2").arg(fromId).arg(content));

    if (fromId == friendId) {
        LOG_INFO("Message is from friend, adding to chat");
        addMessageToChat(friendName, content, timestamp, false, true);

        // If window is visible, send read notification
        if (isVisible()) {
            if (!messagesMarkedAsRead) {
                QJsonObject readNotification = Protocol::MessageStructure::createMessageRead(friendId);
                networkManager.sendMessage(readNotification);
                messagesMarkedAsRead = true;
                emit messagesRead(friendId);
            }
        } else {
            messagesMarkedAsRead = false;
        }
    } else {
        LOG_INFO(QString("Message from %1 doesn't match friend ID %2").arg(fromId).arg(friendId));
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
    if (!hasMoreMessages || isLoadingHistory)
        return;

    QJsonObject request;
    request["type"] = Protocol::MessageType::GET_MORE_HISTORY;
    request["friend_id"] = friendId;
    request["offset"] = currentOffset;

    networkManager.sendMessage(request);
    isLoadingHistory = true;
}

void ChatWindow::onScrollValueChanged(int value)
{
    QScrollBar* scrollBar = ui->chatTextEdit->verticalScrollBar();
    if (value <= scrollBar->maximum() * 0.1) {
        loadMoreHistory();
    }
}

void ChatWindow::onSendMessageClicked()
{
    QString message = ui->messageLineEdit->text().trimmed();
    if (message.isEmpty()) return;

    QJsonObject messageRequest = Protocol::MessageStructure::createMessage(friendId, message);
    QDateTime currentTime = QDateTime::currentDateTime();

    QString sender = networkManager.getUsername();
    bool isOwn = (sender != friendName);

    addMessageToChat(sender, message, currentTime, isOwn, true);
    networkManager.sendMessage(messageRequest);
    ui->messageLineEdit->clear();
}

void ChatWindow::addMessageToChat(const QString& sender, const QString& content,
                                  const QDateTime& timestamp, bool isOwn, bool atEnd)
{
    QString timeStr = timestamp.toString("HH:mm:ss");
    QString messageHtml;

    if (isOwn) {
        messageHtml = QString("\n<div style='text-align: right;'><b>%1</b> [%2]<br>%3</div>")
        .arg(sender)
            .arg(timeStr)
            .arg(content);
    } else {
        messageHtml = QString("\n<div style='text-align: left;'><b>%1</b> [%2]<br>%3</div>")
        .arg(sender)
            .arg(timeStr)
            .arg(content);
    }

    QTextCursor cursor(ui->chatTextEdit->document());
    cursor.movePosition(QTextCursor::End);

    cursor.insertText("\n");
    cursor.insertHtml(messageHtml);

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
        QJsonObject readNotification = Protocol::MessageStructure::createMessageRead(friendId);
        networkManager.sendMessage(readNotification);

        LOG_INFO(QString("Sent read notification for messages from friend ID: %1").arg(friendId));
        messagesMarkedAsRead = true;
        emit messagesRead(friendId);
    }
}
