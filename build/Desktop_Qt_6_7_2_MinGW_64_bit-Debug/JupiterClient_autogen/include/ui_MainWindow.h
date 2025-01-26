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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
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
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *labelsLayout;
    QHBoxLayout *statusLayout;
    QLabel *statusLabel;
    QComboBox *statusComboBox;
    QSpacerItem *horizontalSpacer;
    QLabel *friendsLabel;
    QLabel *logoLabel;
    QSpacerItem *horizontalSpacerRight;
    QListWidget *friendsList;
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
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/icons/sun_icon.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow {\n"
"    background-color: #f5f6fa;\n"
"}\n"
"\n"
"QMenuBar {\n"
"    background-color: #2c3e50;\n"
"    color: white;\n"
"    padding: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QMenuBar::item {\n"
"    padding: 4px 10px;\n"
"    margin: 2px;\n"
"    border-radius: 4px;\n"
"}\n"
"QMenuBar::item:selected {\n"
"    background-color: #34495e;\n"
"}\n"
"QMenuBar::item:pressed {\n"
"    background-color: #2980b9;\n"
"}\n"
"\n"
"QMenu {\n"
"    background-color: #2c3e50;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 5px;\n"
"}\n"
"QMenu::item {\n"
"    padding: 8px 25px;\n"
"    border-radius: 4px;\n"
"}\n"
"QMenu::item:selected {\n"
"    background-color: #2980b9;\n"
"}\n"
"\n"
"QStatusBar {\n"
"    background-color: #2c3e50;\n"
"    color: white;\n"
"    padding: 5px;\n"
"}\n"
"\n"
"QComboBox {\n"
"    background-color: white;\n"
"    border: 2px solid #3498db;\n"
"    border-radius: 6px;\n"
"    padding: 5px 5px 5px 5px;\n"
"    min-height: 25px;\n"
"    max-width: 82px;\n"
"    min-wi"
                        "dth: 82px;\n"
"    font-family: 'Segoe UI';\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"}\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    width: 15px;\n"
"    padding-right: 5px;\n"
"}\n"
"QComboBox::down-arrow {\n"
"    width: 12px;\n"
"    height: 12px;\n"
"}\n"
"QComboBox:hover {\n"
"    border-color: #2980b9;\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"    background-color: white;\n"
"    border: 2px solid #3498db;\n"
"    border-radius: 6px;\n"
"    selection-background-color: #e3f2fd;\n"
"    selection-color: #2c3e50;\n"
"    font-family: 'Segoe UI';\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
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
"QListWidget::item {\n"
"    padding: 8px 5px;\n"
"    margin: 0px;\n"
"    border-radius: 0px;\n"
"    font-family: 'Segoe UI';\n"
""
                        "    font-size: 13px;\n"
"    font-weight: 600;\n"
"}\n"
"QListWidget::item:hover {\n"
"    background-color: #f0f2f5;\n"
"}\n"
"QListWidget::item:selected {\n"
"    background-color: #e3f2fd;\n"
"    color: #2c3e50;\n"
"}\n"
"QListWidget::item:focus {\n"
"    outline: none;\n"
"    border: none;\n"
"}\n"
"\n"
"QLabel {\n"
"    color: #2c3e50;\n"
"    font-weight: bold;\n"
"    font-size: 14px;\n"
"    padding: 5px;\n"
"    font-family: 'Segoe UI';\n"
"}\n"
"\n"
"QScrollBar:vertical {\n"
"    border: none;\n"
"    background-color: #f0f2f5;\n"
"    width: 10px;\n"
"    border-radius: 5px;\n"
"}\n"
"QScrollBar::handle:vertical {\n"
"    background-color: #bdc3c7;\n"
"    border-radius: 5px;\n"
"}\n"
"QScrollBar::handle:vertical:hover {\n"
"    background-color: #95a5a6;\n"
"}\n"
"QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {\n"
"    background: none;\n"
"}\n"
"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {\n"
"    background: none;\n"
"}"));
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
        verticalLayout->setSpacing(12);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(20, 20, 20, 20);
        leftPanel = new QWidget(centralwidget);
        leftPanel->setObjectName("leftPanel");
        verticalLayout_2 = new QVBoxLayout(leftPanel);
        verticalLayout_2->setSpacing(10);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(-1, 0, -1, -1);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        labelsLayout = new QVBoxLayout();
        labelsLayout->setSpacing(15);
        labelsLayout->setObjectName("labelsLayout");
        statusLayout = new QHBoxLayout();
        statusLayout->setSpacing(10);
        statusLayout->setObjectName("statusLayout");
        statusLabel = new QLabel(leftPanel);
        statusLabel->setObjectName("statusLabel");

        statusLayout->addWidget(statusLabel);

        statusComboBox = new QComboBox(leftPanel);
        statusComboBox->setObjectName("statusComboBox");

        statusLayout->addWidget(statusComboBox);

        horizontalSpacer = new QSpacerItem(60, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        statusLayout->addItem(horizontalSpacer);


        labelsLayout->addLayout(statusLayout);

        friendsLabel = new QLabel(leftPanel);
        friendsLabel->setObjectName("friendsLabel");

        labelsLayout->addWidget(friendsLabel);


        horizontalLayout->addLayout(labelsLayout);

        logoLabel = new QLabel(leftPanel);
        logoLabel->setObjectName("logoLabel");
        logoLabel->setMinimumSize(QSize(80, 80));
        logoLabel->setMaximumSize(QSize(80, 80));
        logoLabel->setPixmap(QPixmap(QString::fromUtf8(":/resources/logo/jupiter_logo.png")));
        logoLabel->setScaledContents(true);

        horizontalLayout->addWidget(logoLabel);

        horizontalSpacerRight = new QSpacerItem(0, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacerRight);


        verticalLayout_2->addLayout(horizontalLayout);

        friendsList = new QListWidget(leftPanel);
        friendsList->setObjectName("friendsList");

        verticalLayout_2->addWidget(friendsList);


        verticalLayout->addWidget(leftPanel);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 400, 36));
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
        logoLabel->setText(QString());
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
