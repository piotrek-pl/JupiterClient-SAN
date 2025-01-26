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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *topContainer;
    QVBoxLayout *topLayout;
    QSpacerItem *verticalSpacer;
    QLabel *logoLabel;
    QSpacerItem *verticalSpacer_3;
    QFormLayout *formLayout;
    QLabel *usernameLabel;
    QLineEdit *usernameLineEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QLabel *emailLabel;
    QLineEdit *emailLineEdit;
    QSpacerItem *verticalSpacer_2;
    QPushButton *loginButton;
    QSpacerItem *verticalSpacer_4;
    QPushButton *registerButton;
    QPushButton *backToLoginButton;
    QLabel *statusLabel;

    void setupUi(QWidget *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName("LoginWindow");
        LoginWindow->resize(400, 420);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LoginWindow->sizePolicy().hasHeightForWidth());
        LoginWindow->setSizePolicy(sizePolicy);
        LoginWindow->setMinimumSize(QSize(400, 420));
        LoginWindow->setMaximumSize(QSize(400, 420));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/icons/sun_icon.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        LoginWindow->setWindowIcon(icon);
        LoginWindow->setStyleSheet(QString::fromUtf8("QWidget {\n"
"    background-color: #f5f6fa;\n"
"}\n"
"\n"
"QLabel {\n"
"    color: #2c3e50;\n"
"    font-weight: bold;\n"
"    font-size: 14px;\n"
"    font-family: 'Segoe UI';\n"
"}\n"
"\n"
"QLineEdit {\n"
"    background-color: white;\n"
"    border: 2px solid #3498db;\n"
"    border-radius: 6px;\n"
"    padding: 5px;\n"
"    min-height: 25px;\n"
"    font-family: 'Segoe UI';\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border-color: #2980b9;\n"
"}\n"
"\n"
"QPushButton {\n"
"    background-color: #3498db;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 6px;\n"
"    padding: 8px;\n"
"    font-family: 'Segoe UI';\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    min-height: 30px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #2980b9;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #2475a7;\n"
"}\n"
"\n"
"#titleLabel {\n"
"    font-size: 24px;\n"
"    color: #2c3e50;\n"
"    padding: 10px;\n"
"}\n"
"\n"
"#statusLabel {\n"
"    font"
                        "-size: 12px;\n"
"    color: #7f8c8d;\n"
"}"));
        verticalLayout = new QVBoxLayout(LoginWindow);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(40, 5, 40, 5);
        topContainer = new QWidget(LoginWindow);
        topContainer->setObjectName("topContainer");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(topContainer->sizePolicy().hasHeightForWidth());
        topContainer->setSizePolicy(sizePolicy1);
        topLayout = new QVBoxLayout(topContainer);
        topLayout->setSpacing(2);
        topLayout->setObjectName("topLayout");
        topLayout->setContentsMargins(0, 0, 0, 0);
        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        topLayout->addItem(verticalSpacer);

        logoLabel = new QLabel(topContainer);
        logoLabel->setObjectName("logoLabel");
        logoLabel->setEnabled(true);
        logoLabel->setMinimumSize(QSize(80, 80));
        logoLabel->setMaximumSize(QSize(80, 80));
        logoLabel->setPixmap(QPixmap(QString::fromUtf8(":/resources/logo/jupiter_logo.png")));
        logoLabel->setScaledContents(true);
        logoLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        topLayout->addWidget(logoLabel, 0, Qt::AlignmentFlag::AlignHCenter);


        verticalLayout->addWidget(topContainer);

        verticalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout->addItem(verticalSpacer_3);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        formLayout->setHorizontalSpacing(10);
        formLayout->setVerticalSpacing(10);
        usernameLabel = new QLabel(LoginWindow);
        usernameLabel->setObjectName("usernameLabel");

        formLayout->setWidget(0, QFormLayout::LabelRole, usernameLabel);

        usernameLineEdit = new QLineEdit(LoginWindow);
        usernameLineEdit->setObjectName("usernameLineEdit");
        usernameLineEdit->setMaxLength(32);

        formLayout->setWidget(0, QFormLayout::FieldRole, usernameLineEdit);

        passwordLabel = new QLabel(LoginWindow);
        passwordLabel->setObjectName("passwordLabel");

        formLayout->setWidget(1, QFormLayout::LabelRole, passwordLabel);

        passwordLineEdit = new QLineEdit(LoginWindow);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);

        formLayout->setWidget(1, QFormLayout::FieldRole, passwordLineEdit);

        emailLabel = new QLabel(LoginWindow);
        emailLabel->setObjectName("emailLabel");

        formLayout->setWidget(2, QFormLayout::LabelRole, emailLabel);

        emailLineEdit = new QLineEdit(LoginWindow);
        emailLineEdit->setObjectName("emailLineEdit");

        formLayout->setWidget(2, QFormLayout::FieldRole, emailLineEdit);


        verticalLayout->addLayout(formLayout);

        verticalSpacer_2 = new QSpacerItem(20, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout->addItem(verticalSpacer_2);

        loginButton = new QPushButton(LoginWindow);
        loginButton->setObjectName("loginButton");

        verticalLayout->addWidget(loginButton);

        verticalSpacer_4 = new QSpacerItem(20, 3, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout->addItem(verticalSpacer_4);

        registerButton = new QPushButton(LoginWindow);
        registerButton->setObjectName("registerButton");

        verticalLayout->addWidget(registerButton);

        backToLoginButton = new QPushButton(LoginWindow);
        backToLoginButton->setObjectName("backToLoginButton");
        backToLoginButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: transparent;\n"
"    color: #3498db;\n"
"}\n"
"QPushButton:hover {\n"
"    color: #2980b9;\n"
"    background-color: transparent;\n"
"}\n"
"QPushButton:pressed {\n"
"    color: #2475a7;\n"
"    background-color: transparent;\n"
"}"));

        verticalLayout->addWidget(backToLoginButton);

        statusLabel = new QLabel(LoginWindow);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(statusLabel);


        retranslateUi(LoginWindow);

        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QWidget *LoginWindow)
    {
        LoginWindow->setWindowTitle(QCoreApplication::translate("LoginWindow", "Jupiter Client - Login", nullptr));
        logoLabel->setText(QString());
        usernameLabel->setText(QCoreApplication::translate("LoginWindow", "Username:", nullptr));
        usernameLineEdit->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Enter your username", nullptr));
        passwordLabel->setText(QCoreApplication::translate("LoginWindow", "Password:", nullptr));
        passwordLineEdit->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Enter your password", nullptr));
        emailLabel->setText(QCoreApplication::translate("LoginWindow", "Email:", nullptr));
        emailLineEdit->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Enter your email", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginWindow", "Login", nullptr));
        registerButton->setText(QCoreApplication::translate("LoginWindow", "Create Account", nullptr));
        backToLoginButton->setText(QCoreApplication::translate("LoginWindow", "Back to Login", nullptr));
        statusLabel->setText(QCoreApplication::translate("LoginWindow", "Ready", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
