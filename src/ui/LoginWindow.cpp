#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include "../network/Protocol.h"  // Dodaj bibliotekę Protocol
#include <QJsonDocument>  // Dodaj nagłówek QJsonDocument

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow) {
    ui->setupUi(this);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginButtonClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterButtonClicked);
    connect(&socket, &QTcpSocket::readyRead, this, &LoginWindow::onReadyRead);
    connect(&socket, &QTcpSocket::connected, this, &LoginWindow::onConnected);
    connect(&socket, QOverload<QTcpSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &LoginWindow::onError); // Dodaj obsługę błędów

    checkConnection(); // Dodaj wywołanie metody checkConnection
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::checkConnection() {
    qInfo() << "Attempting to connect to the server...";
    socket.connectToHost("127.0.0.1", 1234); // Adres IP i port serwera
}

void LoginWindow::onConnected() {
    qInfo() << "Connected to the server";
    ui->statusLabel->setText("Connected to the server");
}

void LoginWindow::onError(QTcpSocket::SocketError socketError) {
    qWarning() << "Connection error:" << socketError << "Error string:" << socket.errorString();
    ui->statusLabel->setText("Connection error: " + socket.errorString());
}

void LoginWindow::onLoginButtonClicked() {
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    QJsonObject json = Protocol::MessageStructure::createLoginRequest(username, password);
    socket.write(QJsonDocument(json).toJson());
}

void LoginWindow::onRegisterButtonClicked() {
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QString email = ui->emailLineEdit->text();

    QJsonObject json = Protocol::MessageStructure::createMessage(username.toInt(), password);
    socket.write(QJsonDocument(json).toJson());
}

void LoginWindow::onReadyRead() {
    QByteArray data = socket.readAll();
    if (!data.isEmpty()) {
        qDebug() << "Received from server:" << data;
    } else {
        qDebug() << "No data received from server.";
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject json = doc.object();

    QString type = json["type"].toString();
    if (type == Protocol::MessageType::LOGIN_RESPONSE && json["status"] == "success") {
        emit loginSuccessful();
    } else if (type == Protocol::MessageType::REGISTER_RESPONSE && json["status"] == "success") {
        emit registrationSuccessful();
    } else if (type == Protocol::MessageType::PING) {
        qInfo() << "Ping received: connection is alive.";
        sendPong(json["timestamp"].toInteger());
    } else {
        qWarning() << "Received unknown message type or error";
    }
}

void LoginWindow::sendPong(qint64 timestamp) {
    QJsonObject pongMessage = Protocol::MessageStructure::createPong(timestamp);
    socket.write(QJsonDocument(pongMessage).toJson());
    socket.flush();
    qDebug() << "Pong sent";
}

QTcpSocket* LoginWindow::getSocket() {
    return &socket;
}
