#pragma once

#include <QWidget>
#include <QScopedPointer>

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget {
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();

private:
    QScopedPointer<Ui::ChatWindow> ui;
};
