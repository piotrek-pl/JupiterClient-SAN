/**
 * @file InvitationsDialog.h
 * @brief Dialog for managing friend invitations
 * @author piotrek-pl
 * @date 2025-01-24 22:52:06
 */

#ifndef INVITATIONSDIALOG_H
#define INVITATIONSDIALOG_H

#include <QDialog>
#include <QList>
#include <QJsonObject>
#include <QSet>  // Dodajemy include dla QSet
#include "network/NetworkManager.h"

namespace Ui {
class InvitationsDialog;
}

class InvitationsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InvitationsDialog(NetworkManager& networkManager, QWidget *parent = nullptr);
    ~InvitationsDialog();

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
    void setupConnections();
    void updateReceivedInvitations(const QJsonArray& invitations);
    void updateSentInvitations(const QJsonArray& invitations);
    void updateInvitationsCount();
    void clearLists();
    void emitPendingInvitations();
    QSet<int> getPendingUserIds() const;

private:
    Ui::InvitationsDialog *ui;
    NetworkManager& networkManager;

    struct Invitation {
        int requestId;
        int userId;      // Dodane nowe pole
        QString username;
        qint64 timestamp;
    };

    QList<Invitation> receivedInvitations;
    QList<Invitation> sentInvitations;
};

#endif // INVITATIONSDIALOG_H
