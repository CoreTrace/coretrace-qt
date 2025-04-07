#include "../includes/ide.hpp"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application information
    app.setApplicationName("CoreTrace IDE");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("CoreTrace");
    
    // Get IDE instance and start it
    IDE* ide = IDE::getInstance();
    ide->start();
    
    // Start the event loop
    int result = app.exec();
    
    // Clean up
    ide->stop();
    
    return result;
} 
