/**
 * @file LoginWindow.cpp
 * @brief Login window class implementation
 * @author piotrek-pl
 * @date 2025-01-21 12:58:36
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

    // Ustaw placeholdery
    ui->usernameLineEdit->setPlaceholderText("Enter username");
    ui->passwordLineEdit->setPlaceholderText("Enter password");
    ui->emailLineEdit->setPlaceholderText("Enter email for registration");

    // Ukryj pole email na początku
    ui->emailLineEdit->setVisible(false);
    ui->emailLabel->setVisible(false);

    // Dodaj walidację
    ui->usernameLineEdit->setMaxLength(32);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    updateStatus("Connecting to server...");
}

void LoginWindow::setupNetworkConnections()
{
    connect(&networkManager, &NetworkManager::connectionStatusChanged,
            this, &LoginWindow::onConnectionStatusChanged);
    connect(&networkManager, &NetworkManager::error,
            this, &LoginWindow::onNetworkError);
    connect(&networkManager, &NetworkManager::loginSuccessful,
            this, &LoginWindow::onLoginSuccess);
    connect(&networkManager, &NetworkManager::registrationSuccessful,
            this, &LoginWindow::onRegistrationSuccess);
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
        ui->emailLineEdit->setVisible(true);
        ui->emailLabel->setVisible(true);
        ui->registerButton->setText("Complete Registration");
        updateStatus("Please fill all fields to register");
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

void LoginWindow::onConnectionStatusChanged(const QString& status)
{
    updateStatus(status);
}

void LoginWindow::onNetworkError(const QString& error)
{
    QMessageBox::warning(this, "Connection Error", error);
    updateStatus("Error: " + error);
}

void LoginWindow::onLoginSuccess()
{
    updateStatus("Login successful");
    emit loginSuccessful();
}

void LoginWindow::onRegistrationSuccess()
{
    ui->emailLineEdit->setVisible(false);
    ui->emailLabel->setVisible(false);
    ui->registerButton->setText("Register");
    updateStatus("Registration successful - please login");
}

void LoginWindow::updateStatus(const QString& status)
{
    LOG_INFO(QString("Status update: %1").arg(status));
    ui->statusLabel->setText(status);
}
