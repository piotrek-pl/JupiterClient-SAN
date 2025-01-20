#pragma once

#include <QWidget>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

    QTcpSocket* getSocket();  // Dodaj metodę getSocket

signals:
    void loginSuccessful();  // Dodaj sygnał loginSuccessful
    void registrationSuccessful();  // Dodaj sygnał registrationSuccessful

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
    void onReadyRead();
    void onConnected();
    void onError(QTcpSocket::SocketError socketError);  // Dodaj deklarację metody onError

private:
    void checkConnection();  // Dodaj deklarację metody checkConnection
    void sendPong(qint64 timestamp);  // Dodaj deklarację metody sendPong

    Ui::LoginWindow *ui;
    QTcpSocket socket;  // Zmienna socket
};
