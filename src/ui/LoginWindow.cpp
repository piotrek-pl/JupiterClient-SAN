/**
 * @file LoginWindow.cpp
 * @brief Login window class implementation
 * @author piotrek-pl
 * @date 2025-01-26 09:17:06
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

void LoginWindow::validateLoginFields()
{
    if (ui->emailLineEdit->isVisible()) {
        return; // jesteśmy w trybie rejestracji
    }

    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    bool isValid = !username.isEmpty() && !password.isEmpty();
    ui->loginButton->setEnabled(isValid);

    if (!isValid) {
        updateStatus("Please enter both username and password");
    } else {
        updateStatus("Ready to login");
    }
}

void LoginWindow::validateRegistrationFields()
{
    if (!ui->emailLineEdit->isVisible()) {
        return; // jesteśmy w trybie logowania
    }

    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString email = ui->emailLineEdit->text().trimmed();

    bool isValid = true;
    QString message;

    // Sprawdź czy wszystkie pola są wypełnione
    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        isValid = false;
        message = "Please fill all fields";
    }

    // Sprawdź format emaila
    if (!email.isEmpty()) {
        QRegularExpression emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        QRegularExpressionMatch match = emailRegex.match(email);
        if (!match.hasMatch()) {
            isValid = false;
            message = "Invalid email format";
        }
    }

    ui->registerButton->setEnabled(isValid);

    // Dodajemy warunek sprawdzający czy nie jesteśmy w trakcie błędu rejestracji
    if (!message.isEmpty() && !isRegistering) {
        updateStatus(message);
    } else if (isValid && !isRegistering) {
        updateStatus("Ready to register");
    }
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

    // Dodaj walidację email podczas wpisywania
    connect(ui->emailLineEdit, &QLineEdit::textChanged, [this](const QString& text) {
        if (ui->emailLineEdit->isVisible()) {
            QRegularExpression emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
            QRegularExpressionMatch match = emailRegex.match(text.trimmed());

            QPalette palette = ui->emailLineEdit->palette();
            if (!text.isEmpty() && !match.hasMatch()) {
                palette.setColor(QPalette::Text, Qt::red);
            } else {
                palette.setColor(QPalette::Text, Qt::black);
            }
            ui->emailLineEdit->setPalette(palette);
            validateRegistrationFields();
        }
    });

    // Dodaj walidację dla wszystkich pól
    connect(ui->usernameLineEdit, &QLineEdit::textChanged, [this]() {
        if (ui->emailLineEdit->isVisible()) {
            validateRegistrationFields();
        } else {
            validateLoginFields();
        }
    });

    connect(ui->passwordLineEdit, &QLineEdit::textChanged, [this]() {
        if (ui->emailLineEdit->isVisible()) {
            validateRegistrationFields();
        } else {
            validateLoginFields();
        }
    });

    // Enter key handling
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
    ui->registerButton->setEnabled(true);

    // Wyczyść wszystkie pola
    ui->usernameLineEdit->clear();
    ui->passwordLineEdit->clear();
    ui->emailLineEdit->clear();

    validateLoginFields();  // Sprawdź stan pól logowania
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

    isRegistering = false; // Reset flagi
    isConnecting = false;  // Dodaj to

    // Wyczyść wszystkie pola
    ui->usernameLineEdit->clear();
    ui->passwordLineEdit->clear();
    ui->emailLineEdit->clear();

    updateButtonStates(true);  // Dodaj to przed validateRegistrationFields
    updateStatus("Fill all fields to create account");
    validateRegistrationFields();
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

    // Sprawdzenie czy pola nie są puste
    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        updateStatus("Please fill all fields");
        return;
    }

    // Walidacja formatu email
    QRegularExpression emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    QRegularExpressionMatch match = emailRegex.match(email);

    if (!match.hasMatch()) {
        updateStatus("Invalid email format");
        ui->emailLineEdit->setFocus();
        return;
    }

    // Sprawdzenie połączenia z serwerem
    if (!networkManager.isConnected()) {
        updateStatus("Not connected to server - trying to reconnect...");
        networkManager.connectToServer();
        return;
    }

    // Wyłącz przycisk i ustaw flagę
    isRegistering = true;
    updateButtonStates(false);
    updateStatus("Registering...");

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
    if (isRegistering) {
        isRegistering = false;
        updateButtonStates(true);
        validateRegistrationFields(); // Przywróć walidację pól
        updateStatus(error); // Pokaż błąd z serwera w statusLabel
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
    isConnecting = false;  // Dodaj to
    switchToLoginMode();
    updateButtonStates(true);  // Dodaj to
    updateStatus("Registration successful - please login");
}

void LoginWindow::updateStatus(const QString& status)
{
    ui->statusLabel->setText(status);
}

void LoginWindow::updateButtonStates(bool enabled)
{
    if (isConnecting || isRegistering) {
        enabled = false;
    }

    ui->loginButton->setEnabled(enabled);
    ui->registerButton->setEnabled(enabled);
    ui->backToLoginButton->setEnabled(true); // Zawsze aktywny, żeby można było wrócić
    ui->usernameLineEdit->setEnabled(enabled);
    ui->passwordLineEdit->setEnabled(enabled);

    if (ui->emailLineEdit->isVisible()) {
        ui->emailLineEdit->setEnabled(enabled);
        if (enabled) {
            validateRegistrationFields();
        }
    } else {
        validateLoginFields();
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
