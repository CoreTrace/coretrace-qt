#include "../includes/ide.hpp"
#include "../includes/audit_factory.hpp"
#include <QDebug>
#include <QIcon>
#include <QDir>

IDE* IDE::instance = nullptr;

IDE* IDE::getInstance() {
    if (!instance) {
        instance = new IDE();
    }
    return instance;
}

IDE::IDE() {
    // Initialize components
    mainWindow = new MainWindow();
    auditService = AuditFactory::createAuditService();
    projectManager = new ProjectManager();
    
    // Add toolbar buttons
    mainWindow->addToolButton(
        "Open Project",
        "Open a project directory",
        QIcon::fromTheme("document-open"),
        SLOT(selectProjectFolder()),
        projectManager
    );
    
    mainWindow->addToolSeparator();
    
    // Set up initial connections
    // TODO: Add signal/slot connections for project management and audit results
}

IDE::~IDE() {
    // Cleanup is handled by stop()
}

void IDE::start() {
    qDebug() << "Starting CoreTrace IDE...";
    
    // Initialize the main window
    mainWindow->show();
    
    // TODO: Load any saved project state or settings
    
    qDebug() << "CoreTrace IDE started successfully";
}

void IDE::stop() {
    qDebug() << "Stopping CoreTrace IDE...";
    
    // Close all open projects
    for (Project* project : projectManager->getProjects()) {
        projectManager->closeProject(project);
    }
    
    // Clean up components
    delete mainWindow;
    mainWindow = nullptr;
    
    delete auditService;
    auditService = nullptr;
    
    delete projectManager;
    projectManager = nullptr;
    
    // Clear the singleton instance
    delete instance;
    instance = nullptr;
    
    qDebug() << "CoreTrace IDE stopped successfully";
} 