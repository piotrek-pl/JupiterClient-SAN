#include "ChatWindow.h"
#include "ui_ChatWindow.h"
#include <QScrollBar>

ChatWindow::ChatWindow(const QString& friendName, int friendId, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWindow)
    , networkManager(NetworkManager::getInstance())
    , friendName(friendName)
    , friendId(friendId)
{
    ui->setupUi(this);
    setWindowTitle("Chat with " + friendName);
    initializeUI();

    // Połącz sygnały
    connect(&networkManager, &NetworkManager::messageReceived,
            this, &ChatWindow::onMessageReceived);
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::initializeUI()
{
    // Połącz przycisk wysyłania
    connect(ui->sendButton, &QPushButton::clicked, this, &ChatWindow::onSendMessageClicked);
    connect(ui->messageLineEdit, &QLineEdit::returnPressed, this, &ChatWindow::onSendMessageClicked);

    ui->chatTextEdit->setReadOnly(true);
}

void ChatWindow::onSendMessageClicked()
{
    QString message = ui->messageLineEdit->text().trimmed();
    if (message.isEmpty()) return;

    QJsonObject messageRequest = Protocol::MessageStructure::createMessage(friendId, message);
    networkManager.sendMessage(messageRequest);
    ui->messageLineEdit->clear();
}

void ChatWindow::onMessageReceived(const QJsonObject& json)
{
    QString type = json["type"].toString();

    if (type == Protocol::MessageType::MESSAGE_RESPONSE) {
        QString sender = json["sender"].toString();
        QString content = json["content"].toString();
        QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(json["timestamp"].toInteger());

        // Sprawdź czy wiadomość jest od/do tego znajomego
        if (sender == friendName || json["recipient"].toString() == friendName) {
            bool isOwn = (sender != friendName);
            addMessageToChat(sender, content, timestamp, isOwn);
        }
    }
}

void ChatWindow::addMessageToChat(const QString& sender, const QString& content,
                                  const QDateTime& timestamp, bool isOwn)
{
    QString timeStr = timestamp.toString("HH:mm:ss");
    QString messageHtml;

    if (isOwn) {
        messageHtml = QString("<div style='text-align: right;'><b>%1</b> [%2]<br>%3</div>")
        .arg(sender).arg(timeStr).arg(content);
    } else {
        messageHtml = QString("<div style='text-align: left;'><b>%1</b> [%2]<br>%3</div>")
        .arg(sender).arg(timeStr).arg(content);
    }

    ui->chatTextEdit->append(messageHtml);
    ui->chatTextEdit->verticalScrollBar()->setValue(
        ui->chatTextEdit->verticalScrollBar()->maximum());
}

void ChatWindow::processMessage(const QJsonObject& message)
{
    onMessageReceived(message);
}
