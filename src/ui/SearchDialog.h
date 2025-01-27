/**
 * @file SearchDialog.h
 * @brief Search dialog for finding and adding friends
 * @author piotrek-pl
 * @date 2025-01-27 09:11:39
 */

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QSet>
#include "network/NetworkManager.h"

namespace Ui {
class SearchDialog;
}

class MainWindow;

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(NetworkManager& networkManager, MainWindow* parent = nullptr);
    ~SearchDialog();

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
    // Initialization methods
    void initializeUI();
    void setupSearchTimer();
    void setupConnections();

    // Context menu methods
    void createContextMenuForFriend(QMenu& menu, const QString& username);
    void createContextMenuForPendingInvitation(QMenu& menu);
    void createContextMenuForNewUser(QMenu& menu, int userId, const QString& username);

    // Response handlers
    void handleAddFriendResponse(const QJsonObject& response);
    void handleInvitationExistsResponse(const QJsonObject& response);

    // Helper methods
    void sendFriendRequest(int userId, const QString& username);

private:
    Ui::SearchDialog *ui;
    NetworkManager& networkManager;
    MainWindow* mainWindow;
    QTimer* searchTimer;
    QSet<int> pendingInvitations;
    QJsonObject lastSearchResponse;
};

#endif // SEARCHDIALOG_H
