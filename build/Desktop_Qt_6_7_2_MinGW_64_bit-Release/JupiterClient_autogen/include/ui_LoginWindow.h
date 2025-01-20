/********************************************************************************
** Form generated from reading UI file 'LoginWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QVBoxLayout *verticalLayout;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QLineEdit *emailLineEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QLabel *statusLabel;

    void setupUi(QWidget *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName("LoginWindow");
        LoginWindow->resize(400, 300);
        verticalLayout = new QVBoxLayout(LoginWindow);
        verticalLayout->setObjectName("verticalLayout");
        usernameLineEdit = new QLineEdit(LoginWindow);
        usernameLineEdit->setObjectName("usernameLineEdit");

        verticalLayout->addWidget(usernameLineEdit);

        passwordLineEdit = new QLineEdit(LoginWindow);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        verticalLayout->addWidget(passwordLineEdit);

        emailLineEdit = new QLineEdit(LoginWindow);
        emailLineEdit->setObjectName("emailLineEdit");

        verticalLayout->addWidget(emailLineEdit);

        loginButton = new QPushButton(LoginWindow);
        loginButton->setObjectName("loginButton");

        verticalLayout->addWidget(loginButton);

        registerButton = new QPushButton(LoginWindow);
        registerButton->setObjectName("registerButton");

        verticalLayout->addWidget(registerButton);

        statusLabel = new QLabel(LoginWindow);
        statusLabel->setObjectName("statusLabel");

        verticalLayout->addWidget(statusLabel);


        retranslateUi(LoginWindow);

        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QWidget *LoginWindow)
    {
        LoginWindow->setWindowTitle(QCoreApplication::translate("LoginWindow", "Login", nullptr));
        usernameLineEdit->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Username", nullptr));
        passwordLineEdit->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Password", nullptr));
        emailLineEdit->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Email", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginWindow", "Login", nullptr));
        registerButton->setText(QCoreApplication::translate("LoginWindow", "Register", nullptr));
        statusLabel->setText(QCoreApplication::translate("LoginWindow", "Connecting to server...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
