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
#include <QtGui/QIcon>
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
        ChatWindow->setMinimumSize(QSize(400, 500));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/icons/sun_icon.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        ChatWindow->setWindowIcon(icon);
        ChatWindow->setStyleSheet(QString::fromUtf8("QWidget {\n"
"    background-color: #f5f6fa;\n"
"}\n"
"\n"
"QTextEdit {\n"
"    background-color: white;\n"
"    border: 2px solid #e1e8ed;\n"
"    border-radius: 8px;\n"
"    padding: 10px;\n"
"    font-family: 'Segoe UI';\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QLineEdit {\n"
"    background-color: white;\n"
"    border: 2px solid #3498db;\n"
"    border-radius: 6px;\n"
"    padding: 8px;\n"
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
"    padding: 8px 20px;\n"
"    font-family: 'Segoe UI';\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    min-height: 30px;\n"
"    min-width: 80px;\n"
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
"QScrollBar:vertical {\n"
" "
                        "   border: none;\n"
"    background-color: #f0f2f5;\n"
"    width: 10px;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"    background-color: #bdc3c7;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical:hover {\n"
"    background-color: #95a5a6;\n"
"}\n"
"\n"
"QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {\n"
"    background: none;\n"
"}\n"
"\n"
"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {\n"
"    background: none;\n"
"}"));
        verticalLayout = new QVBoxLayout(ChatWindow);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(20, 20, 20, 20);
        chatTextEdit = new QTextEdit(ChatWindow);
        chatTextEdit->setObjectName("chatTextEdit");
        chatTextEdit->setReadOnly(true);

        verticalLayout->addWidget(chatTextEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(10);
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
        messageLineEdit->setPlaceholderText(QCoreApplication::translate("ChatWindow", "Type your message...", nullptr));
        sendButton->setText(QCoreApplication::translate("ChatWindow", "Send", nullptr));
        (void)ChatWindow;
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
