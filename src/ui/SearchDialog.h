/**
 * @file SearchDialog.h
 * @brief Search dialog class definition
 * @author piotrek-pl
 * @date 2025-01-24 10:25:16
 */

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QTimer>
#include <QJsonObject>
#include "network/NetworkManager.h"

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(NetworkManager& networkManager, QWidget *parent = nullptr);

public slots:
    void onSearchResponse(const QJsonObject& response);
    void handleServerResponse(const QJsonObject& response);

private slots:
    void onSearchTextChanged(const QString& text);
    void performSearch();
    void showContextMenu(const QPoint& pos);

private:
    void setupUI();

    NetworkManager& networkManager;
    QLineEdit* searchEdit;
    QListWidget* resultsList;
    QTimer* searchTimer;
};

#endif // SEARCHDIALOG_H
