#include "../includes/Application.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Application application;
    application.showFullScreen();

    return app.exec();
}
