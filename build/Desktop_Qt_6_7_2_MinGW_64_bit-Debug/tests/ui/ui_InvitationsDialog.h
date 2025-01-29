/********************************************************************************
** Form generated from reading UI file 'InvitationsDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INVITATIONSDIALOG_H
#define UI_INVITATIONSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InvitationsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *receivedTab;
    QVBoxLayout *verticalLayout_2;
    QListWidget *receivedList;
    QHBoxLayout *horizontalLayout;
    QPushButton *acceptButton;
    QPushButton *rejectButton;
    QWidget *sentTab;
    QVBoxLayout *verticalLayout_3;
    QListWidget *sentList;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *cancelButton;

    void setupUi(QDialog *InvitationsDialog)
    {
        if (InvitationsDialog->objectName().isEmpty())
            InvitationsDialog->setObjectName("InvitationsDialog");
        InvitationsDialog->resize(400, 500);
        InvitationsDialog->setMinimumSize(QSize(400, 500));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/icons/sun_icon.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        InvitationsDialog->setWindowIcon(icon);
        InvitationsDialog->setStyleSheet(QString::fromUtf8("QDialog {\n"
"    background-color: #f5f6fa;\n"
"}\n"
"\n"
"QTabWidget::pane {\n"
"    border: 2px solid #e1e8ed;\n"
"    border-radius: 8px;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    background-color: #f5f6fa;\n"
"    color: #2c3e50;\n"
"    padding: 8px 20px;\n"
"    border: 2px solid #e1e8ed;\n"
"    border-bottom: none;\n"
"    border-top-left-radius: 6px;\n"
"    border-top-right-radius: 6px;\n"
"    font-family: 'Segoe UI';\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QTabBar::tab:selected {\n"
"    background-color: white;\n"
"    border-bottom: none;\n"
"}\n"
"\n"
"QTabBar::tab:!selected {\n"
"    margin-top: 2px;\n"
"}\n"
"\n"
"QListWidget {\n"
"    background-color: white;\n"
"    border: 2px solid #e1e8ed;\n"
"    border-radius: 8px;\n"
"    padding: 5px;\n"
"    font-family: 'Segoe UI';\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    outline: 0;\n"
"}\n"
"\n"
"QListWidget::item {\n"
"    padding: 8px 5px;\n"
"    margin: 0px;\n"
"    bord"
                        "er-radius: 0px;\n"
"    font-family: 'Segoe UI';\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QListWidget::item:hover {\n"
"    background-color: #f0f2f5;\n"
"}\n"
"\n"
"QListWidget::item:selected {\n"
"    background-color: #e3f2fd;\n"
"    color: #2c3e50;\n"
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
"    border: none;\n"
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
""
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
        verticalLayout = new QVBoxLayout(InvitationsDialog);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(20, 20, 20, 20);
        tabWidget = new QTabWidget(InvitationsDialog);
        tabWidget->setObjectName("tabWidget");
        receivedTab = new QWidget();
        receivedTab->setObjectName("receivedTab");
        verticalLayout_2 = new QVBoxLayout(receivedTab);
        verticalLayout_2->setSpacing(10);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(10, 10, 10, 10);
        receivedList = new QListWidget(receivedTab);
        receivedList->setObjectName("receivedList");

        verticalLayout_2->addWidget(receivedList);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName("horizontalLayout");
        acceptButton = new QPushButton(receivedTab);
        acceptButton->setObjectName("acceptButton");

        horizontalLayout->addWidget(acceptButton);

        rejectButton = new QPushButton(receivedTab);
        rejectButton->setObjectName("rejectButton");

        horizontalLayout->addWidget(rejectButton);


        verticalLayout_2->addLayout(horizontalLayout);

        tabWidget->addTab(receivedTab, QString());
        sentTab = new QWidget();
        sentTab->setObjectName("sentTab");
        verticalLayout_3 = new QVBoxLayout(sentTab);
        verticalLayout_3->setSpacing(10);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(10, 10, 10, 10);
        sentList = new QListWidget(sentTab);
        sentList->setObjectName("sentList");

        verticalLayout_3->addWidget(sentList);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        cancelButton = new QPushButton(sentTab);
        cancelButton->setObjectName("cancelButton");

        horizontalLayout_2->addWidget(cancelButton);


        verticalLayout_3->addLayout(horizontalLayout_2);

        tabWidget->addTab(sentTab, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(InvitationsDialog);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(InvitationsDialog);
    } // setupUi

    void retranslateUi(QDialog *InvitationsDialog)
    {
        InvitationsDialog->setWindowTitle(QCoreApplication::translate("InvitationsDialog", "Friend Invitations", nullptr));
        acceptButton->setText(QCoreApplication::translate("InvitationsDialog", "Accept", nullptr));
        rejectButton->setText(QCoreApplication::translate("InvitationsDialog", "Reject", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(receivedTab), QCoreApplication::translate("InvitationsDialog", "Received (0)", nullptr));
        cancelButton->setText(QCoreApplication::translate("InvitationsDialog", "Cancel", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(sentTab), QCoreApplication::translate("InvitationsDialog", "Sent (0)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InvitationsDialog: public Ui_InvitationsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INVITATIONSDIALOG_H
