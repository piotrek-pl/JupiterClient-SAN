/**
 * @file InvitationsDialog.h
 * @brief Dialog for managing friend invitations
 * @author piotrek-pl
 * @date 2025-01-24 10:53:14
 */

#ifndef INVITATIONSDIALOG_H
#define INVITATIONSDIALOG_H

#include <QDialog>
#include <QList>
#include <QJsonObject>
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

private:
    Ui::InvitationsDialog *ui;
    NetworkManager& networkManager;

    struct Invitation {
        int requestId;
        QString username;
        qint64 timestamp;
    };

    QList<Invitation> receivedInvitations;
    QList<Invitation> sentInvitations;
};

#endif // INVITATIONSDIALOG_H
