#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "../network/Protocol.h"  // Dodaj bibliotekę Protocol
#include <QJsonDocument>  // Dodaj nagłówek QJsonDocument

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    socket(nullptr) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setSocket(QTcpSocket *sharedSocket) {
    socket = sharedSocket;
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
}

void MainWindow::onReadyRead() {
    if (!socket) return;

    QByteArray data = socket->readAll();
    if (!data.isEmpty()) {
        qDebug() << "Received from server:" << data;
    } else {
        qDebug() << "No data received from server.";
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject json = doc.object();

    QString type = json["type"].toString();
    if (type == Protocol::MessageType::PONG) {
        qInfo() << "Pong received: connection is alive.";
    } else if (type == Protocol::MessageType::PING) {
        qInfo() << "Ping received: connection is alive.";
        sendPong(json["timestamp"].toInteger());
    } else {
        qDebug() << "Received from server:" << data;
        // Obsługa innych typów wiadomości
    }
}

void MainWindow::sendPong(qint64 timestamp) {
    QJsonObject pongMessage = Protocol::MessageStructure::createPong(timestamp);
    socket->write(QJsonDocument(pongMessage).toJson());
    socket->flush();
    qDebug() << "Pong sent";
}
