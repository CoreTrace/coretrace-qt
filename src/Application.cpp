#include "../includes/Application.hpp"

Application::~Application() {
    delete this->mainWindow;
}

Application::Application(QWidget *parent) : QStackedWidget(parent), mainWindow(new Board(this)) {
    addWidget(this->mainWindow);
    setCurrentWidget(this->mainWindow);
}
