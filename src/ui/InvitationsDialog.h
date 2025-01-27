/**
 * @file InvitationsDialog.h
 * @brief Dialog for managing friend invitations
 * @author piotrek-pl
 * @date 2025-01-27 08:43:10
 */

#ifndef INVITATIONSDIALOG_H
#define INVITATIONSDIALOG_H

#include <QDialog>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QListWidget>
#include "network/NetworkManager.h"

namespace Ui {
class InvitationsDialog;
}

class InvitationsDialog : public QDialog {
    Q_OBJECT

public:
    explicit InvitationsDialog(NetworkManager& networkManager, QWidget *parent = nullptr);
    ~InvitationsDialog();

    struct SelectedInvitation {
        bool isValid;
        int requestId;
        QString errorMessage;

        SelectedInvitation(bool valid = false, int id = 0,
                           const QString& error = "Please select an invitation first")
            : isValid(valid), requestId(id), errorMessage(error) {}
    };

signals:
    void pendingInvitationsChanged(const QSet<int>& userIds);
    void invitationStatusChanged(int userId);

public slots:
    void onMessageReceived(const QJsonObject& message);
    void refreshInvitations();

private slots:
    void onTabChanged(int index);
    void onAcceptClicked();
    void onRejectClicked();
    void onCancelClicked();

private:
    // Setup
    void setupConnections();

    // Message handlers
    void handleReceivedInvitationsResponse(const QJsonObject& message);
    void handleSentInvitationsResponse(const QJsonObject& message);
    void handleFriendRequestAcceptResponse(const QJsonObject& message);
    void handleFriendRequestRejectResponse(const QJsonObject& message);
    void handleCancelFriendRequestResponse(const QJsonObject& message);
    void handleInvitationStatusChanged(const QJsonObject& message);
    void handleFriendRequestCancelledNotification(const QJsonObject& message);

    // UI updates
    void updateReceivedInvitations(const QJsonArray& invitations);
    void updateSentInvitations(const QJsonArray& invitations);
    void updateInvitationsCount();
    void clearLists();

    // Helper methods
    void emitPendingInvitations();
    QSet<int> getPendingUserIds() const;
    void showResponseMessage(const QString& title, const QString& message);
    void handleInvitationStatusChange(const QJsonObject& message);
    SelectedInvitation getSelectedInvitation(QListWidget* list);

private:
    Ui::InvitationsDialog *ui;
    NetworkManager& networkManager;

    struct Invitation {
        int requestId;
        int userId;
        QString username;
        qint64 timestamp;
    };

    QList<Invitation> receivedInvitations;
    QList<Invitation> sentInvitations;
};

#endif // INVITATIONSDIALOG_H
