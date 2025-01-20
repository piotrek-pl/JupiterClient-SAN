#pragma once

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setSocket(QTcpSocket *sharedSocket);

private slots:
    void onReadyRead();

private:
    void sendPong(qint64 timestamp);  // Dodaj deklarację metody sendPong

    Ui::MainWindow *ui;
    QTcpSocket *socket;  // Wskaźnik do gniazda
};
