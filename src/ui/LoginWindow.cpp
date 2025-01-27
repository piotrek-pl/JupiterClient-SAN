/**
 * @file LoginWindow.cpp
 * @brief Login window class implementation
 * @author piotrek-pl
 * @date 2025-01-27 08:55:55
 */

#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWindow)
    , networkManager(NetworkManager::getInstance())
    , isConnecting(false)
    , isRegistering(false)
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

// UI Setup Methods
void LoginWindow::setupPlaceholders()
{
    ui->usernameLineEdit->setPlaceholderText("Enter username");
    ui->passwordLineEdit->setPlaceholderText("Enter password");
    ui->emailLineEdit->setPlaceholderText("Enter email for registration");
}

void LoginWindow::setupVisibility()
{
    ui->emailLineEdit->setVisible(false);
    ui->emailLabel->setVisible(false);
    ui->backToLoginButton->setVisible(false);
}

void LoginWindow::setupValidation()
{
    ui->usernameLineEdit->setMaxLength(32);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}

void LoginWindow::setupConnections()
{
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginButtonClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterButtonClicked);
    connect(ui->backToLoginButton, &QPushButton::clicked, this, &LoginWindow::onBackToLoginClicked);

    connect(ui->emailLineEdit, &QLineEdit::textChanged, [this](const QString& text) {
        if (ui->emailLineEdit->isVisible()) {
            QString errorMessage;
            bool isValid = validateEmail(text.trimmed(), errorMessage);

            QPalette palette = ui->emailLineEdit->palette();
            palette.setColor(QPalette::Text, isValid ? Qt::black : Qt::red);
            ui->emailLineEdit->setPalette(palette);
            validateRegistrationFields();
        }
    });

    connect(ui->usernameLineEdit, &QLineEdit::textChanged, [this]() {
        ui->emailLineEdit->isVisible() ? validateRegistrationFields() : validateLoginFields();
    });

    connect(ui->passwordLineEdit, &QLineEdit::textChanged, [this]() {
        ui->emailLineEdit->isVisible() ? validateRegistrationFields() : validateLoginFields();
    });
}

void LoginWindow::setupEnterKeyHandling()
{
    connect(ui->usernameLineEdit, &QLineEdit::returnPressed, [this]() {
        if (ui->emailLineEdit->isVisible()) {
            ui->passwordLineEdit->setFocus();
        } else if (ui->loginButton->isEnabled()) {
            validateAndSubmitLogin();
        }
    });

    connect(ui->passwordLineEdit, &QLineEdit::returnPressed, [this]() {
        if (ui->emailLineEdit->isVisible()) {
            ui->emailLineEdit->setFocus();
        } else if (ui->loginButton->isEnabled()) {
            validateAndSubmitLogin();
        }
    });

    connect(ui->emailLineEdit, &QLineEdit::returnPressed, [this]() {
        if (ui->emailLineEdit->isVisible() && ui->registerButton->isEnabled()) {
            validateAndSubmitRegistration();
        }
    });
}

void LoginWindow::initializeUI()
{
    setupPlaceholders();
    setupVisibility();
    setupValidation();
    setupConnections();
    setupEnterKeyHandling();

    isConnecting = true;
    updateButtonStates(false);
    updateStatus("Connecting to server...");
}

// Validation Methods
bool LoginWindow::validateEmail(const QString& email, QString& errorMessage)
{
    if (email.isEmpty()) {
        errorMessage = "Email is required";
        return false;
    }

    QRegularExpression emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    QRegularExpressionMatch match = emailRegex.match(email);

    if (!match.hasMatch()) {
        errorMessage = "Invalid email format";
        return false;
    }

    return true;
}

bool LoginWindow::areFieldsEmpty(const QString& username, const QString& password, const QString& email)
{
    if (username.isEmpty() || password.isEmpty()) return true;
    if (!email.isNull() && email.isEmpty()) return true;
    return false;
}

void LoginWindow::validateLoginFields()
{
    if (ui->emailLineEdit->isVisible()) return;

    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    bool isValid = !areFieldsEmpty(username, password);
    ui->loginButton->setEnabled(isValid);

    updateStatus(isValid ? "Ready to login" : "Please enter both username and password");
}

void LoginWindow::validateRegistrationFields()
{
    if (!ui->emailLineEdit->isVisible()) return;

    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString email = ui->emailLineEdit->text().trimmed();

    bool isValid = true;
    QString message;

    if (areFieldsEmpty(username, password, email)) {
        isValid = false;
        message = "Please fill all fields";
    } else {
        isValid = validateEmail(email, message);
    }

    ui->registerButton->setEnabled(isValid);

    if (!message.isEmpty() && !isRegistering) {
        updateStatus(message);
    } else if (isValid && !isRegistering) {
        updateStatus("Ready to register");
    }
}

// Form Submission Methods
void LoginWindow::handleLoginSubmission(const QString& username, const QString& password)
{
    if (!networkManager.isConnected()) {
        updateStatus("Not connected to server - trying to reconnect...");
        networkManager.connectToServer();
        return;
    }

    networkManager.login(username, password);
}

void LoginWindow::handleRegistrationSubmission(const QString& username, const QString& password, const QString& email)
{
    if (!networkManager.isConnected()) {
        updateStatus("Not connected to server - trying to reconnect...");
        networkManager.connectToServer();
        return;
    }

    isRegistering = true;
    updateButtonStates(false);
    updateStatus("Registering...");

    networkManager.registerUser(username, password, email);
}

void LoginWindow::validateAndSubmitLogin()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (areFieldsEmpty(username, password)) {
        updateStatus("Please enter both username and password");
        return;
    }

    handleLoginSubmission(username, password);
}

void LoginWindow::validateAndSubmitRegistration()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString email = ui->emailLineEdit->text().trimmed();
    QString errorMessage;

    if (areFieldsEmpty(username, password, email)) {
        updateStatus("Please fill all fields");
        return;
    }

    if (!validateEmail(email, errorMessage)) {
        updateStatus(errorMessage);
        ui->emailLineEdit->setFocus();
        return;
    }

    handleRegistrationSubmission(username, password, email);
}

// Mode Switching Methods
void LoginWindow::clearAllFields()
{
    ui->usernameLineEdit->clear();
    ui->passwordLineEdit->clear();
    ui->emailLineEdit->clear();
}

void LoginWindow::switchToLoginMode()
{
    setWindowTitle("Jupiter Client - Login");
    ui->emailLineEdit->setVisible(false);
    ui->emailLabel->setVisible(false);
    ui->backToLoginButton->setVisible(false);
    ui->loginButton->setVisible(true);
    ui->registerButton->setText("Create Account");
    ui->registerButton->setEnabled(true);

    clearAllFields();
    validateLoginFields();
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

    isRegistering = false;
    isConnecting = false;

    clearAllFields();
    updateButtonStates(true);
    updateStatus("Fill all fields to create account");
    validateRegistrationFields();
}

// Network State Methods
void LoginWindow::handleNetworkState(bool connected, const QString& message)
{
    updateButtonStates(connected);
    updateStatus(message);
}

void LoginWindow::setupNetworkConnections()
{
    disconnect(&networkManager, nullptr, this, nullptr);

    connect(&networkManager, &NetworkManager::connected, this, &LoginWindow::onNetworkConnected);
    connect(&networkManager, &NetworkManager::disconnected, this, &LoginWindow::onNetworkDisconnected);
    connect(&networkManager, &NetworkManager::connectionStatusChanged, this, &LoginWindow::onConnectionStatusChanged);
    connect(&networkManager, &NetworkManager::error, this, &LoginWindow::onNetworkError);
    connect(&networkManager, &NetworkManager::loginSuccessful, this, &LoginWindow::onLoginSuccess);
    connect(&networkManager, &NetworkManager::registrationSuccessful, this, &LoginWindow::onRegistrationSuccess);
}

// Event Handlers
void LoginWindow::onBackToLoginClicked()
{
    switchToLoginMode();
}

void LoginWindow::onLoginButtonClicked()
{
    validateAndSubmitLogin();
}

void LoginWindow::onRegisterButtonClicked()
{
    if (ui->emailLineEdit->isVisible()) {
        validateAndSubmitRegistration();
    } else {
        switchToRegisterMode();
    }
}

void LoginWindow::onNetworkConnected()
{
    isConnecting = false;
    handleNetworkState(true, "Connected to server");
}

void LoginWindow::onNetworkDisconnected()
{
    handleNetworkState(false, "Disconnected from server");
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
    if (isRegistering) {
        isRegistering = false;
        updateButtonStates(true);
        validateRegistrationFields();
        updateStatus(error);
        LOG_DEBUG("Status updated to: " + error);
    } else {
        updateStatus("Error: " + error);
        if (!error.contains("Authentication") && !error.contains("Invalid credentials")) {
            updateButtonStates(false);
        }
    }
}

void LoginWindow::onLoginSuccess()
{
    updateStatus("Login successful");
    emit loginSuccessful();
}

void LoginWindow::onRegistrationSuccess()
{
    isRegistering = false;
    isConnecting = false;
    switchToLoginMode();
    updateButtonStates(true);
    updateStatus("Registration successful - please login");
}

// UI Update Methods
void LoginWindow::updateStatus(const QString& status)
{
    ui->statusLabel->setText(status);
}

void LoginWindow::updateFieldsEnabled(bool enabled)
{
    ui->usernameLineEdit->setEnabled(enabled);
    ui->passwordLineEdit->setEnabled(enabled);
    ui->emailLineEdit->setEnabled(enabled && ui->emailLineEdit->isVisible());
}

void LoginWindow::updateButtonStates(bool enabled)
{
    if (isConnecting || isRegistering) {
        enabled = false;
    }

    ui->loginButton->setEnabled(enabled);
    ui->registerButton->setEnabled(enabled);
    ui->backToLoginButton->setEnabled(true);
    updateFieldsEnabled(enabled);

    if (ui->emailLineEdit->isVisible()) {
        if (enabled) validateRegistrationFields();
    } else {
        validateLoginFields();
    }
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

void LoginWindow::closeEvent(QCloseEvent* event)
{
    disconnect(&networkManager, nullptr, this, nullptr);
    QWidget::closeEvent(event);
}
