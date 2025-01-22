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
#include <QtWidgets/QHBoxLayout>
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
    QHBoxLayout *horizontalLayout;
    QLineEdit *messageLineEdit;
    QPushButton *sendButton;

    void setupUi(QWidget *ChatWindow)
    {
        if (ChatWindow->objectName().isEmpty())
            ChatWindow->setObjectName("ChatWindow");
        ChatWindow->resize(400, 500);
        verticalLayout = new QVBoxLayout(ChatWindow);
        verticalLayout->setObjectName("verticalLayout");
        chatTextEdit = new QTextEdit(ChatWindow);
        chatTextEdit->setObjectName("chatTextEdit");

        verticalLayout->addWidget(chatTextEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        messageLineEdit = new QLineEdit(ChatWindow);
        messageLineEdit->setObjectName("messageLineEdit");

        horizontalLayout->addWidget(messageLineEdit);

        sendButton = new QPushButton(ChatWindow);
        sendButton->setObjectName("sendButton");

        horizontalLayout->addWidget(sendButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ChatWindow);

        QMetaObject::connectSlotsByName(ChatWindow);
    } // setupUi

    void retranslateUi(QWidget *ChatWindow)
    {
        sendButton->setText(QCoreApplication::translate("ChatWindow", "Send", nullptr));
        (void)ChatWindow;
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
