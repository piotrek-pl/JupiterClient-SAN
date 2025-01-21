/**
 * @file LoginWindow.h
 * @brief Login window class definition
 * @author piotrek-pl
 * @date 2025-01-21 12:58:36
 */

#pragma once

#include <QWidget>
#include "network/NetworkManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void loginSuccessful();
    void registrationSuccessful();

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
    void onConnectionStatusChanged(const QString& status);
    void onNetworkError(const QString& error);
    void onLoginSuccess();
    void onRegistrationSuccess();

private:
    void initializeUI();
    void updateStatus(const QString& status);
    void setupNetworkConnections();
    void validateAndSubmitRegistration();
    void validateAndSubmitLogin();

    Ui::LoginWindow *ui;
    NetworkManager& networkManager;
};
