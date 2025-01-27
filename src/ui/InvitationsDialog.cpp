/**
 * @file InvitationsDialog.cpp
 * @brief Dialog for managing friend invitations implementation
 * @author piotrek-pl
 * @date 2025-01-27 08:43:10
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

void InvitationsDialog::handleReceivedInvitationsResponse(const QJsonObject& message)
{
    updateReceivedInvitations(message["invitations"].toArray());
}

void InvitationsDialog::handleSentInvitationsResponse(const QJsonObject& message)
{
    updateSentInvitations(message["invitations"].toArray());
}

void InvitationsDialog::handleFriendRequestAcceptResponse(const QJsonObject& message)
{
    if (message["status"].toString() == "success") {
        handleInvitationStatusChange(message);
        refreshInvitations();
    }
}

void InvitationsDialog::handleFriendRequestRejectResponse(const QJsonObject& message)
{
    if (message["status"].toString() == "success") {
        handleInvitationStatusChange(message);
        refreshInvitations();
    }
    showResponseMessage("Reject Request", message["message"].toString());
}

void InvitationsDialog::handleCancelFriendRequestResponse(const QJsonObject& message)
{
    if (message["status"].toString() == "success") {
        handleInvitationStatusChange(message);
        refreshInvitations();
    }
    showResponseMessage("Cancel Request", message["message"].toString());
}

void InvitationsDialog::handleInvitationStatusChanged(const QJsonObject& message)
{
    handleInvitationStatusChange(message);
    refreshInvitations();
}

void InvitationsDialog::handleFriendRequestCancelledNotification(const QJsonObject& message)
{
    refreshInvitations();
    showResponseMessage("Friend Request Cancelled", "A friend request has been cancelled.");
}

void InvitationsDialog::onMessageReceived(const QJsonObject& message)
{
    QString type = message["type"].toString();

    if (type == Protocol::MessageType::RECEIVED_INVITATIONS_RESPONSE) {
        handleReceivedInvitationsResponse(message);
    }
    else if (type == Protocol::MessageType::SENT_INVITATIONS_RESPONSE) {
        handleSentInvitationsResponse(message);
    }
    else if (type == Protocol::MessageType::FRIEND_REQUEST_ACCEPT_RESPONSE) {
        handleFriendRequestAcceptResponse(message);
    }
    else if (type == Protocol::MessageType::FRIEND_REQUEST_REJECT_RESPONSE) {
        handleFriendRequestRejectResponse(message);
    }
    else if (type == Protocol::MessageType::CANCEL_FRIEND_REQUEST_RESPONSE) {
        handleCancelFriendRequestResponse(message);
    }
    else if (type == Protocol::MessageType::INVITATION_STATUS_CHANGED) {
        handleInvitationStatusChanged(message);
    }
    else if (type == Protocol::MessageType::FRIEND_REQUEST_CANCELLED_NOTIFICATION) {
        handleFriendRequestCancelledNotification(message);
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

InvitationsDialog::SelectedInvitation InvitationsDialog::getSelectedInvitation(QListWidget* list)
{
    QListWidgetItem* currentItem = list->currentItem();
    if (currentItem) {
        return SelectedInvitation(true, currentItem->data(Qt::UserRole).toInt());
    }
    return SelectedInvitation();
}

void InvitationsDialog::onTabChanged(int index)
{
    ui->acceptButton->setVisible(index == 0);
    ui->rejectButton->setVisible(index == 0);
    ui->cancelButton->setVisible(index == 1);
}

void InvitationsDialog::onAcceptClicked()
{
    auto selected = getSelectedInvitation(ui->receivedList);
    if (!selected.isValid) {
        showResponseMessage("Warning", selected.errorMessage);
        return;
    }

    QJsonObject request = Protocol::MessageStructure::createFriendRequestAccept(selected.requestId);
    networkManager.sendMessage(request);
}

void InvitationsDialog::onRejectClicked()
{
    auto selected = getSelectedInvitation(ui->receivedList);
    if (!selected.isValid) {
        showResponseMessage("Warning", selected.errorMessage);
        return;
    }

    QJsonObject request = Protocol::MessageStructure::createFriendRequestReject(selected.requestId);
    networkManager.sendMessage(request);
}

void InvitationsDialog::onCancelClicked()
{
    auto selected = getSelectedInvitation(ui->sentList);
    if (!selected.isValid) {
        showResponseMessage("Warning", selected.errorMessage);
        return;
    }

    QJsonObject request = Protocol::MessageStructure::createCancelFriendRequest(selected.requestId);
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

void InvitationsDialog::showResponseMessage(const QString& title, const QString& message)
{
    QMessageBox::information(this, title, message);
}

void InvitationsDialog::handleInvitationStatusChange(const QJsonObject& message)
{
    if (message.contains("user_id")) {
        emit invitationStatusChanged(message["user_id"].toInt());
    }
}
