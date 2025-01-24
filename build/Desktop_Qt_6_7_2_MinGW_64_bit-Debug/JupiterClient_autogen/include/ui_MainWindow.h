/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSearch;
    QAction *actionSettings;
    QAction *actionExit;
    QAction *actionAbout;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QWidget *leftPanel;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *statusLayout;
    QLabel *statusLabel;
    QComboBox *statusComboBox;
    QLabel *friendsLabel;
    QListWidget *friendsList;
    QPushButton *refreshButton;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(400, 600);
        MainWindow->setMinimumSize(QSize(400, 600));
        actionSearch = new QAction(MainWindow);
        actionSearch->setObjectName("actionSearch");
        actionSettings = new QAction(MainWindow);
        actionSettings->setObjectName("actionSettings");
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        leftPanel = new QWidget(centralwidget);
        leftPanel->setObjectName("leftPanel");
        verticalLayout_2 = new QVBoxLayout(leftPanel);
        verticalLayout_2->setObjectName("verticalLayout_2");
        statusLayout = new QHBoxLayout();
        statusLayout->setObjectName("statusLayout");
        statusLabel = new QLabel(leftPanel);
        statusLabel->setObjectName("statusLabel");

        statusLayout->addWidget(statusLabel);

        statusComboBox = new QComboBox(leftPanel);
        statusComboBox->setObjectName("statusComboBox");

        statusLayout->addWidget(statusComboBox);


        verticalLayout_2->addLayout(statusLayout);

        friendsLabel = new QLabel(leftPanel);
        friendsLabel->setObjectName("friendsLabel");

        verticalLayout_2->addWidget(friendsLabel);

        friendsList = new QListWidget(leftPanel);
        friendsList->setObjectName("friendsList");

        verticalLayout_2->addWidget(friendsList);

        refreshButton = new QPushButton(leftPanel);
        refreshButton->setObjectName("refreshButton");

        verticalLayout_2->addWidget(refreshButton);


        verticalLayout->addWidget(leftPanel);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 400, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionSearch);
        menuFile->addAction(actionSettings);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Jupiter Client", nullptr));
        actionSearch->setText(QCoreApplication::translate("MainWindow", "Search Users", nullptr));
        actionSettings->setText(QCoreApplication::translate("MainWindow", "Settings", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        statusLabel->setText(QCoreApplication::translate("MainWindow", "Status:", nullptr));
        friendsLabel->setText(QCoreApplication::translate("MainWindow", "Friends:", nullptr));
        refreshButton->setText(QCoreApplication::translate("MainWindow", "Refresh List", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
