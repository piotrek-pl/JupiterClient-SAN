/********************************************************************************
** Form generated from reading UI file 'ChatWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatWindow
{
public:
    QVBoxLayout *verticalLayout;
    QTextEdit *chatTextEdit;
    QLineEdit *messageLineEdit;
    QPushButton *sendButton;

    void setupUi(QWidget *ChatWindow)
    {
        if (ChatWindow->objectName().isEmpty())
            ChatWindow->setObjectName("ChatWindow");
        ChatWindow->resize(400, 300);
        verticalLayout = new QVBoxLayout(ChatWindow);
        verticalLayout->setObjectName("verticalLayout");
        chatTextEdit = new QTextEdit(ChatWindow);
        chatTextEdit->setObjectName("chatTextEdit");

        verticalLayout->addWidget(chatTextEdit);

        messageLineEdit = new QLineEdit(ChatWindow);
        messageLineEdit->setObjectName("messageLineEdit");

        verticalLayout->addWidget(messageLineEdit);

        sendButton = new QPushButton(ChatWindow);
        sendButton->setObjectName("sendButton");

        verticalLayout->addWidget(sendButton);


        retranslateUi(ChatWindow);

        QMetaObject::connectSlotsByName(ChatWindow);
    } // setupUi

    void retranslateUi(QWidget *ChatWindow)
    {
        ChatWindow->setWindowTitle(QCoreApplication::translate("ChatWindow", "ChatWindow", nullptr));
        sendButton->setText(QCoreApplication::translate("ChatWindow", "Send", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
