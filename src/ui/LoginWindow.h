/**
 * @file LoginWindow.h
 * @brief Login window class definition
 * @author piotrek-pl
 * @date 2025-01-27 08:55:55
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
    // UI Setup
    void initializeUI();
    void setupPlaceholders();
    void setupVisibility();
    void setupValidation();
    void setupConnections();
    void setupEnterKeyHandling();
    void setupNetworkConnections();
    void setupConnectionHandling();

    // UI Updates
    void updateStatus(const QString& status);
    void updateButtonStates(bool enabled);
    void updateFieldsEnabled(bool enabled);
    void clearAllFields();

    // Validation
    void validateLoginFields();
    void validateRegistrationFields();
    bool validateEmail(const QString& email, QString& errorMessage);
    bool areFieldsEmpty(const QString& username, const QString& password, const QString& email = QString());

    // Form Submission
    void validateAndSubmitRegistration();
    void validateAndSubmitLogin();
    void handleLoginSubmission(const QString& username, const QString& password);
    void handleRegistrationSubmission(const QString& username, const QString& password, const QString& email);

    // Mode Switching
    void switchToLoginMode();
    void switchToRegisterMode();

    // Network State
    void handleNetworkState(bool connected, const QString& message);

private:
    Ui::LoginWindow *ui;
    NetworkManager& networkManager;
    bool isConnecting;
    bool isRegistering;

protected:
    void closeEvent(QCloseEvent* event) override;
};
