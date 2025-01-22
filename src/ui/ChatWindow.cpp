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

void ChatWindow::loadInitialHistory()
{
    QJsonObject request;
    request["type"] = Protocol::MessageType::GET_LATEST_MESSAGES;  // Nowy typ wiadomości
    request["friend_id"] = friendId;
    request["limit"] = Protocol::ChatHistory::MESSAGE_BATCH_SIZE;  // Domyślnie 20

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

void ChatWindow::onScrollValueChanged(int value)
{
    QScrollBar* scrollBar = ui->chatTextEdit->verticalScrollBar();
    if (value <= scrollBar->maximum() * 0.1) { // Gdy przewiniemy do górnych 10%
        loadMoreHistory();
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
        QJsonArray messages = json["messages"].toArray();
        int oldScrollPos = ui->chatTextEdit->verticalScrollBar()->value();
        int oldMax = ui->chatTextEdit->verticalScrollBar()->maximum();

        // Tekst do wstawienia
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

            if (type == Protocol::MessageType::MORE_HISTORY_RESPONSE) {
                // Dla starszych wiadomości dodaj na początek
                newMessages = messageHtml + newMessages;
            } else {
                // Dla najnowszych wiadomości dodaj na koniec
                newMessages += messageHtml;
            }
        }

        // Wstaw tekst w odpowiednie miejsce
        QTextCursor cursor(ui->chatTextEdit->document());

        if (type == Protocol::MessageType::MORE_HISTORY_RESPONSE) {
            // Wstaw na początku
            cursor.movePosition(QTextCursor::Start);
            cursor.insertHtml(newMessages);

            // Jeśli są już jakieś wiadomości, dodaj pojedynczy znak nowego wiersza
            if (!ui->chatTextEdit->toPlainText().isEmpty()) {
                cursor.movePosition(QTextCursor::Start);
                cursor.insertText("\n");
            }
        } else {
            // Wstaw na końcu
            cursor.movePosition(QTextCursor::End);
            // Tutaj, zamiast sprawdzania czy pole jest puste, zawsze wstaw nowy wiersz
            cursor.insertText("\n");
            cursor.insertHtml(newMessages);
        }

        currentOffset += messages.size();
        hasMoreMessages = json["has_more"].toBool();
        isLoadingHistory = false;

        if (type == Protocol::MessageType::MORE_HISTORY_RESPONSE) {
            // Zachowaj pozycję przewijania dla starszych wiadomości
            int newMax = ui->chatTextEdit->verticalScrollBar()->maximum();
            ui->chatTextEdit->verticalScrollBar()->setValue(oldScrollPos + (newMax - oldMax));
        } else {
            // Przewiń na dół dla początkowej historii
            ui->chatTextEdit->verticalScrollBar()->setValue(
                ui->chatTextEdit->verticalScrollBar()->maximum());
        }
    }
    else if (type == Protocol::MessageType::MESSAGE_RESPONSE)
    {
        QString sender = json["sender"].toString();
        QString content = json["content"].toString();
        QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(json["timestamp"].toInteger());

        if (sender == friendName || json["recipient"].toString() == friendName) {
            bool isOwn = (sender != friendName);
            addMessageToChat(sender, content, timestamp, isOwn, true);
        }
    }
    else if (type == Protocol::MessageType::NEW_MESSAGES)
    {
        QString content = json["content"].toString();
        int fromId = json["from"].toInt();
        QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(json["timestamp"].toDouble()));

        LOG_INFO(QString("Processing new message from user %1: %2").arg(fromId).arg(content));

        if (fromId == friendId) {
            LOG_INFO("Message is from friend, adding to chat");
            // Użyj friendName jako sender, ponieważ wiemy, że wiadomość jest od przyjaciela
            addMessageToChat(friendName, content, timestamp, false, true);
        } else {
            LOG_INFO(QString("Message from %1 doesn't match friend ID %2").arg(fromId).arg(friendId));
        }
    }
}

void ChatWindow::onSendMessageClicked()
{
    QString message = ui->messageLineEdit->text().trimmed();
    if (message.isEmpty()) return;

    QJsonObject messageRequest = Protocol::MessageStructure::createMessage(friendId, message);

    // Zapisz czas wysłania
    QDateTime currentTime = QDateTime::currentDateTime();

    // Użyj tej samej nazwy użytkownika co w systemie
    QString sender = networkManager.getUsername();
    bool isOwn = (sender != friendName);

    // Wyświetl wiadomość lokalnie
    addMessageToChat(sender, message, currentTime, isOwn, true);

    // Wyślij wiadomość
    networkManager.sendMessage(messageRequest);

    // Wyczyść pole tekstowe
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

    // Dodaj tylko początkową nową linię
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
