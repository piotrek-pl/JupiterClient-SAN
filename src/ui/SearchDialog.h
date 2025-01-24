/**
 * @file SearchDialog.h
 * @brief Search dialog class header
 * @author piotrek-pl
 * @date 2025-01-24 12:46:36
 */

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QTimer>

class NetworkManager;
class MainWindow;

class SearchDialog : public QDialog {
    Q_OBJECT
public:
    SearchDialog(NetworkManager& networkManager, MainWindow* parent = nullptr);
    void onSearchResponse(const QJsonObject& response);

private slots:
    void onSearchTextChanged(const QString& text);
    void performSearch();
    void showContextMenu(const QPoint& pos);
    void handleServerResponse(const QJsonObject& response);  // Dodana deklaracja

signals:
    void friendRequestSent();

private:
    void setupUI();

    NetworkManager& networkManager;
    QLineEdit* searchEdit;
    QListWidget* resultsList;
    QTimer* searchTimer;
    MainWindow* mainWindow;
};

#endif // SEARCHDIALOG_H
