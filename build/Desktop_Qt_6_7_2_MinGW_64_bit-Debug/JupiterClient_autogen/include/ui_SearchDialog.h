/********************************************************************************
** Form generated from reading UI file 'SearchDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEARCHDIALOG_H
#define UI_SEARCHDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SearchDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLineEdit *searchEdit;
    QListWidget *resultsList;

    void setupUi(QDialog *SearchDialog)
    {
        if (SearchDialog->objectName().isEmpty())
            SearchDialog->setObjectName("SearchDialog");
        SearchDialog->resize(300, 400);
        SearchDialog->setMinimumSize(QSize(300, 400));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/icons/sun_icon.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        SearchDialog->setWindowIcon(icon);
        SearchDialog->setStyleSheet(QString::fromUtf8("QDialog {\n"
"    background-color: #f5f6fa;\n"
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
"    border-radius: 0px;\n"
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
"QListWidget::item:focus {\n"
"    outline: none;"
                        "\n"
"    border: none;\n"
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
        verticalLayout = new QVBoxLayout(SearchDialog);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(20, 20, 20, 20);
        searchEdit = new QLineEdit(SearchDialog);
        searchEdit->setObjectName("searchEdit");

        verticalLayout->addWidget(searchEdit);

        resultsList = new QListWidget(SearchDialog);
        resultsList->setObjectName("resultsList");

        verticalLayout->addWidget(resultsList);


        retranslateUi(SearchDialog);

        QMetaObject::connectSlotsByName(SearchDialog);
    } // setupUi

    void retranslateUi(QDialog *SearchDialog)
    {
        SearchDialog->setWindowTitle(QCoreApplication::translate("SearchDialog", "Search Users", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("SearchDialog", "Enter username to search...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SearchDialog: public Ui_SearchDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEARCHDIALOG_H
