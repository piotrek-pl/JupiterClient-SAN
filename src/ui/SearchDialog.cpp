/**
 * @file SearchDialog.cpp
 * @brief Search dialog class implementation
 * @author piotrek-pl
 * @date 2025-01-24 08:21:15
 */

#include "SearchDialog.h"
#include <QVBoxLayout>
#include <QMenu>
#include "network/Protocol.h"

SearchDialog::SearchDialog(NetworkManager& networkManager, QWidget *parent)
    : QDialog(parent)
    , networkManager(networkManager)
{
    setWindowTitle("Search Users");
    setMinimumWidth(300);
    setMinimumHeight(400);

    setupUI();

    searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(500);
    connect(searchTimer, &QTimer::timeout, this, &SearchDialog::performSearch);
}

void SearchDialog::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Enter username to search...");
    layout->addWidget(searchEdit);

    resultsList = new QListWidget(this);
    resultsList->setContextMenuPolicy(Qt::CustomContextMenu);
    layout->addWidget(resultsList);

    connect(searchEdit, &QLineEdit::textChanged,
            this, &SearchDialog::onSearchTextChanged);
    connect(resultsList, &QWidget::customContextMenuRequested,
            this, &SearchDialog::showContextMenu);
}

void SearchDialog::onSearchTextChanged(const QString& text)
{
    if (text.length() >= 3) {
        searchTimer->start();
    } else {
        resultsList->clear();
    }
}

void SearchDialog::performSearch()
{
    QString query = searchEdit->text();
    if (query.length() >= 3) {
        QJsonObject searchRequest = Protocol::MessageStructure::createSearchUsersRequest(query);
        networkManager.sendMessage(searchRequest);
    }
}

void SearchDialog::onSearchResponse(const QJsonObject& response)
{
    if (response["type"].toString() != Protocol::MessageType::SEARCH_USERS_RESPONSE) {
        return;
    }

    resultsList->clear();
    QJsonArray users = response["users"].toArray();

    for (const QJsonValue& userVal : users) {
        QJsonObject user = userVal.toObject();
        QListWidgetItem* item = new QListWidgetItem(user["username"].toString());
        item->setData(Qt::UserRole, user["id"].toInt());
        resultsList->addItem(item);
    }
}

void SearchDialog::showContextMenu(const QPoint& pos)
{
    QListWidgetItem* item = resultsList->itemAt(pos);
    if (!item) return;

    QMenu contextMenu(this);
    QAction* inviteAction = contextMenu.addAction("Add to Friends");

    QAction* selectedAction = contextMenu.exec(resultsList->mapToGlobal(pos));
    if (selectedAction == inviteAction) {
        // Tu później dodamy wysyłanie zaproszenia
        // int userId = item->data(Qt::UserRole).toInt();
        // QString username = item->text();
    }
}
