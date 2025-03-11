#pragma once
#include "./MainBoard.hpp"

class Application : public QStackedWidget {
    Q_OBJECT

public:
    ~Application();
    Application(QWidget *parent = nullptr);

private slots:
    void showMainWindow() {
        setCurrentWidget(mainWindow);
    }

private:
    Board *mainWindow = new Board(this);
};