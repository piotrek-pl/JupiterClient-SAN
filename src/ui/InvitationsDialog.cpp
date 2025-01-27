/**
 * @file InvitationsDialog.cpp
 * @brief Dialog for managing friend invitations implementation
 * @author piotrek-pl
 * @date 2025-01-24 10:54:24
 */

#include "InvitationsDialog.h"
#include "ui_InvitationsDialog.h"
#include "network/Protocol.h"
#include <QMessageBox>
#include <QDateTime>

InvitationsDialog::InvitationsDialog(NetworkManager& networkManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InvitationsDialog)
    , networkManager(networkManager)
{
    ui->setupUi(this);
    setWindowTitle("Friend Invitations");
    setupConnections();
}

InvitationsDialog::~InvitationsDialog()
{
    delete ui;
}

void InvitationsDialog::setupConnections()
{
    connect(ui->tabWidget, &QTabWidget::currentChanged,
            this, &InvitationsDialog::onTabChanged);

    connect(ui->acceptButton, &QPushButton::clicked,
            this, &InvitationsDialog::onAcceptClicked);

    connect(ui->rejectButton, &QPushButton::clicked,
            this, &InvitationsDialog::onRejectClicked);

    connect(ui->cancelButton, &QPushButton::clicked,
            this, &InvitationsDialog::onCancelClicked);
}

void InvitationsDialog::refreshInvitations()
{
    QJsonObject receivedRequest = Protocol::MessageStructure::createGetReceivedInvitationsRequest();
    networkManager.sendMessage(receivedRequest);

    QJsonObject sentRequest = Protocol::MessageStructure::createGetSentInvitationsRequest();
    networkManager.sendMessage(sentRequest);
}

void InvitationsDialog::onMessageReceived(const QJsonObject& message)
{
    QString type = message["type"].toString();

    if (type == Protocol::MessageType::RECEIVED_INVITATIONS_RESPONSE) {
        updateReceivedInvitations(message["invitations"].toArray());
    }
    else if (type == Protocol::MessageType::SENT_INVITATIONS_RESPONSE) {
        updateSentInvitations(message["invitations"].toArray());
    }
    else if (type == Protocol::MessageType::FRIEND_REQUEST_ACCEPT_RESPONSE) {
        if (message["status"].toString() == "success") {
            if (message.contains("user_id")) {
                int userId = message["user_id"].toInt();
                emit invitationStatusChanged(userId);
            }
            refreshInvitations();
        }
        //QString resultMessage = message["message"].toString();
        //QMessageBox::information(this, "Accept Request", resultMessage);
    }
    else if (type == Protocol::MessageType::FRIEND_REQUEST_REJECT_RESPONSE) {
        if (message["status"].toString() == "success") {
            if (message.contains("user_id")) {
                int userId = message["user_id"].toInt();
                emit invitationStatusChanged(userId);
            }
            refreshInvitations();
        }
        QString resultMessage = message["message"].toString();
        QMessageBox::information(this, "Reject Request", resultMessage);
    }
    else if (type == Protocol::MessageType::CANCEL_FRIEND_REQUEST_RESPONSE) {
        if (message["status"].toString() == "success") {
            if (message.contains("user_id")) {
                int userId = message["user_id"].toInt();
                emit invitationStatusChanged(userId);
            }
            refreshInvitations();
        }
        QString resultMessage = message["message"].toString();
        QMessageBox::information(this, "Cancel Request", resultMessage);
    }
    else if (type == Protocol::MessageType::INVITATION_STATUS_CHANGED) {
        int userId = message["user_id"].toInt();
        emit invitationStatusChanged(userId);
        refreshInvitations();
    }
    else if (type == Protocol::MessageType::FRIEND_REQUEST_CANCELLED_NOTIFICATION) {
        int requestId = message["request_id"].toInt();
        int fromUserId = message["from_user_id"].toInt();

        // Odśwież listy zaproszeń
        refreshInvitations();

        // Opcjonalnie: Pokaż powiadomienie użytkownikowi
        QMessageBox::information(this, "Friend Request Cancelled",
                                 "A friend request has been cancelled.");
    }
}

void InvitationsDialog::updateReceivedInvitations(const QJsonArray& invitations)
{
    ui->receivedList->clear();
    receivedInvitations.clear();

    for (const QJsonValue &value : invitations) {
        QJsonObject inv = value.toObject();
        Invitation invitation;
        invitation.requestId = inv["request_id"].toInt();
        invitation.username = inv["username"].toString();
        invitation.timestamp = inv["timestamp"].toInteger();

        receivedInvitations.append(invitation);

        QString timeStr = QDateTime::fromMSecsSinceEpoch(invitation.timestamp)
                              .toString("yyyy-MM-dd HH:mm:ss");
        QString displayText = QString("%1 (%2)").arg(invitation.username, timeStr);
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, invitation.requestId);
        ui->receivedList->addItem(item);
    }

    updateInvitationsCount();
}

void InvitationsDialog::updateSentInvitations(const QJsonArray& invitations)
{
    ui->sentList->clear();
    sentInvitations.clear();
    QSet<int> pendingUserIds;

    for (const QJsonValue &value : invitations) {
        QJsonObject inv = value.toObject();
        Invitation invitation;
        invitation.requestId = inv["request_id"].toInt();
        invitation.userId = inv["user_id"].toInt();
        invitation.username = inv["username"].toString();
        invitation.timestamp = inv["timestamp"].toInteger();

        sentInvitations.append(invitation);
        pendingUserIds.insert(invitation.userId);

        QString timeStr = QDateTime::fromMSecsSinceEpoch(invitation.timestamp)
                              .toString("yyyy-MM-dd HH:mm:ss");
        QString displayText = QString("%1 (%2)").arg(invitation.username, timeStr);
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, invitation.requestId);
        item->setData(Qt::UserRole + 1, invitation.userId);
        ui->sentList->addItem(item);
    }

    updateInvitationsCount();
    emit pendingInvitationsChanged(pendingUserIds);
}

void InvitationsDialog::onTabChanged(int index)
{
    ui->acceptButton->setVisible(index == 0);
    ui->rejectButton->setVisible(index == 0);
    ui->cancelButton->setVisible(index == 1);
}

void InvitationsDialog::onAcceptClicked()
{
    QListWidgetItem* currentItem = ui->receivedList->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "Warning", "Please select an invitation first");
        return;
    }

    int requestId = currentItem->data(Qt::UserRole).toInt();
    QJsonObject request = Protocol::MessageStructure::createFriendRequestAccept(requestId);
    networkManager.sendMessage(request);
}

void InvitationsDialog::onRejectClicked()
{
    QListWidgetItem* currentItem = ui->receivedList->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "Warning", "Please select an invitation first");
        return;
    }

    int requestId = currentItem->data(Qt::UserRole).toInt();
    QJsonObject request = Protocol::MessageStructure::createFriendRequestReject(requestId);
    networkManager.sendMessage(request);
}

void InvitationsDialog::onCancelClicked()
{
    QListWidgetItem* currentItem = ui->sentList->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "Warning", "Please select an invitation first");
        return;
    }

    int requestId = currentItem->data(Qt::UserRole).toInt();
    QJsonObject request = Protocol::MessageStructure::createCancelFriendRequest(requestId);
    networkManager.sendMessage(request);
}

void InvitationsDialog::updateInvitationsCount()
{
    ui->tabWidget->setTabText(0, QString("Received (%1)").arg(receivedInvitations.size()));
    ui->tabWidget->setTabText(1, QString("Sent (%1)").arg(sentInvitations.size()));
}

void InvitationsDialog::clearLists()
{
    ui->receivedList->clear();
    ui->sentList->clear();
    receivedInvitations.clear();
    sentInvitations.clear();
    updateInvitationsCount();
}
