/**
 * @file SearchDialog.h
 * @brief Search dialog class definition
 * @author piotrek-pl
 * @date 2025-01-24 08:21:15
 */

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QTimer>
#include "network/NetworkManager.h"

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(NetworkManager& networkManager, QWidget *parent = nullptr);
    void onSearchResponse(const QJsonObject& response);

private slots:
    void onSearchTextChanged(const QString& text);
    void showContextMenu(const QPoint& pos);
    void performSearch();

private:
    void setupUI();

    QLineEdit* searchEdit;
    QListWidget* resultsList;
    NetworkManager& networkManager;
    QTimer* searchTimer;
};
