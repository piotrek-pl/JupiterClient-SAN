/**
 * @file LoginWindow.h
 * @brief Login window class definition
 * @author piotrek-pl
 * @date 2025-01-26 08:53:26
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
    void onBackToLoginClicked();
    void onConnectionStatusChanged(const QString& status);
    void onNetworkError(const QString& error);
    void onLoginSuccess();
    void onRegistrationSuccess();
    void onNetworkConnected();
    void onNetworkDisconnected();

private:
    void initializeUI();
    void updateStatus(const QString& status);
    void setupNetworkConnections();
    void validateAndSubmitRegistration();
    void validateAndSubmitLogin();
    void updateButtonStates(bool enabled);
    void setupConnectionHandling();
    void switchToLoginMode();
    void switchToRegisterMode();

    Ui::LoginWindow *ui;
    NetworkManager& networkManager;
    bool isConnecting;

protected:
    void closeEvent(QCloseEvent* event) override;
};
