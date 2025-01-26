/**
 * @file LoginWindow.cpp
 * @brief Login window class implementation
 * @author piotrek-pl
 * @date 2025-01-26 08:53:26
 */

#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWindow)
    , networkManager(NetworkManager::getInstance())
{
    ui->setupUi(this);
    setWindowTitle("Jupiter Client - Login");

    initializeUI();
    setupNetworkConnections();

    LOG_INFO("LoginWindow initialized");
}

LoginWindow::~LoginWindow()
{
    delete ui;
    LOG_INFO("LoginWindow destroyed");
}

void LoginWindow::initializeUI()
{
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginButtonClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterButtonClicked);
    connect(ui->backToLoginButton, &QPushButton::clicked, this, &LoginWindow::onBackToLoginClicked);

    // Ustaw placeholdery
    ui->usernameLineEdit->setPlaceholderText("Enter username");
    ui->passwordLineEdit->setPlaceholderText("Enter password");
    ui->emailLineEdit->setPlaceholderText("Enter email for registration");

    // Ukryj elementy początkowe
    ui->emailLineEdit->setVisible(false);
    ui->emailLabel->setVisible(false);
    ui->backToLoginButton->setVisible(false);

    // Dodaj walidację
    ui->usernameLineEdit->setMaxLength(32);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    // Początkowo wyłącz przyciski
    isConnecting = true;
    updateButtonStates(false);
    updateStatus("Connecting to server...");
}

void LoginWindow::setupNetworkConnections()
{
    disconnect(&networkManager, nullptr, this, nullptr);

    connect(&networkManager, &NetworkManager::connected,
            this, &LoginWindow::onNetworkConnected);
    connect(&networkManager, &NetworkManager::disconnected,
            this, &LoginWindow::onNetworkDisconnected);
    connect(&networkManager, &NetworkManager::connectionStatusChanged,
            this, &LoginWindow::onConnectionStatusChanged);
    connect(&networkManager, &NetworkManager::error,
            this, &LoginWindow::onNetworkError);
    connect(&networkManager, &NetworkManager::loginSuccessful,
            this, &LoginWindow::onLoginSuccess);
    connect(&networkManager, &NetworkManager::registrationSuccessful,
            this, &LoginWindow::onRegistrationSuccess);
}

void LoginWindow::switchToLoginMode()
{
    setWindowTitle("Jupiter Client - Login");
    ui->emailLineEdit->setVisible(false);
    ui->emailLabel->setVisible(false);
    ui->backToLoginButton->setVisible(false);
    ui->loginButton->setVisible(true);
    ui->registerButton->setText("Create Account");

    // Wyczyść pola
    ui->emailLineEdit->clear();
    updateStatus("Enter your credentials to login");
}

void LoginWindow::switchToRegisterMode()
{
    setWindowTitle("Jupiter Client - Registration");
    ui->emailLineEdit->setVisible(true);
    ui->emailLabel->setVisible(true);
    ui->backToLoginButton->setVisible(true);
    ui->loginButton->setVisible(false);
    ui->registerButton->setText("Register");
    updateStatus("Fill all fields to create account");
}

void LoginWindow::onBackToLoginClicked()
{
    switchToLoginMode();
}

void LoginWindow::onLoginButtonClicked()
{
    validateAndSubmitLogin();
}

void LoginWindow::validateAndSubmitLogin()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        updateStatus("Please enter both username and password");
        return;
    }

    if (!networkManager.isConnected()) {
        updateStatus("Not connected to server - trying to reconnect...");
        networkManager.connectToServer();
        return;
    }

    networkManager.login(username, password);
}

void LoginWindow::onRegisterButtonClicked()
{
    if (ui->emailLineEdit->isVisible()) {
        validateAndSubmitRegistration();
    } else {
        switchToRegisterMode();
    }
}

void LoginWindow::validateAndSubmitRegistration()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString email = ui->emailLineEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        updateStatus("Please fill all fields");
        return;
    }

    if (!networkManager.isConnected()) {
        updateStatus("Not connected to server - trying to reconnect...");
        networkManager.connectToServer();
        return;
    }

    networkManager.registerUser(username, password, email);
}

void LoginWindow::onNetworkConnected()
{
    isConnecting = false;
    updateButtonStates(true);
    updateStatus("Connected to server");
}

void LoginWindow::onNetworkDisconnected()
{
    updateButtonStates(false);
    updateStatus("Disconnected from server");
}

void LoginWindow::onConnectionStatusChanged(const QString& status)
{
    updateStatus(status);
    if (status.contains("Connecting")) {
        isConnecting = true;
        updateButtonStates(false);
    }
}

void LoginWindow::onNetworkError(const QString& error)
{
    updateStatus("Error: " + error);
    if (!error.contains("Authentication") && !error.contains("Invalid credentials")) {
        updateButtonStates(false);
    }
}

void LoginWindow::onLoginSuccess()
{
    updateStatus("Login successful");
    emit loginSuccessful();
}

void LoginWindow::onRegistrationSuccess()
{
    switchToLoginMode();
    updateStatus("Registration successful - please login");
}

void LoginWindow::updateStatus(const QString& status)
{
    ui->statusLabel->setText(status);
}

void LoginWindow::updateButtonStates(bool enabled)
{
    if (isConnecting) {
        enabled = false;
    }

    ui->loginButton->setEnabled(enabled);
    ui->registerButton->setEnabled(enabled);
    ui->backToLoginButton->setEnabled(enabled);
    ui->usernameLineEdit->setEnabled(enabled);
    ui->passwordLineEdit->setEnabled(enabled);
    if (ui->emailLineEdit->isVisible()) {
        ui->emailLineEdit->setEnabled(enabled);
    }
}

void LoginWindow::closeEvent(QCloseEvent* event)
{
    disconnect(&networkManager, nullptr, this, nullptr);
    QWidget::closeEvent(event);
}

void LoginWindow::setupConnectionHandling()
{
    isConnecting = true;
    updateButtonStates(false);

    connect(&networkManager, &NetworkManager::connected, this, [this]() {
        isConnecting = false;
        updateButtonStates(true);
    });

    connect(&networkManager, &NetworkManager::disconnected, this, [this]() {
        updateButtonStates(false);
    });
}
