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
        verticalLayout = new QVBoxLayout(InvitationsDialog);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(InvitationsDialog);
        tabWidget->setObjectName("tabWidget");
        receivedTab = new QWidget();
        receivedTab->setObjectName("receivedTab");
        verticalLayout_2 = new QVBoxLayout(receivedTab);
        verticalLayout_2->setObjectName("verticalLayout_2");
        receivedList = new QListWidget(receivedTab);
        receivedList->setObjectName("receivedList");

        verticalLayout_2->addWidget(receivedList);

        horizontalLayout = new QHBoxLayout();
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
        verticalLayout_3->setObjectName("verticalLayout_3");
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
