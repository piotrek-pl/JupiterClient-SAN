/**
 * @file SearchDialog.h
 * @brief Search dialog for finding and adding friends
 * @author piotrek-pl
 * @date 2025-01-24 22:52:38
 */

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QTimer>
#include <QSet>
#include "network/NetworkManager.h"

class MainWindow;

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(NetworkManager& networkManager, MainWindow* parent = nullptr);

signals:
    void friendRequestSent();

public slots:
    void updatePendingInvitations(const QSet<int>& userIds);
    void onInvitationStatusChanged(int userId);
    void onSearchResponse(const QJsonObject& response);

private slots:
    void onSearchTextChanged(const QString& text);
    void performSearch();
    void showContextMenu(const QPoint& pos);
    void handleServerResponse(const QJsonObject& response);

private:
    void setupUI();
    void sendFriendRequest(int userId, const QString& username);

private:
    NetworkManager& networkManager;
    MainWindow* mainWindow;
    QLineEdit* searchEdit;
    QListWidget* resultsList;
    QTimer* searchTimer;
    QSet<int> pendingInvitations;  // Przechowuje ID użytkowników z aktywnymi zaproszeniami
    QJsonObject lastSearchResponse; // Przechowuje ostatnią odpowiedź wyszukiwania
};

#endif // SEARCHDIALOG_H
