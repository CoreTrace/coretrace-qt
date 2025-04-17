#include "../includes/ide.hpp"
#include "../includes/audit_factory.hpp"
#include "../includes/update_service.hpp"
#include <QDebug>
#include <QIcon>
#include <QDir>
#include <QMessageBox>

IDE* IDE::instance = nullptr;

IDE* IDE::getInstance() {
    if (!instance) {
        instance = new IDE();
    }
    return instance;
}

IDE::IDE() : QObject(nullptr) {
    // Initialize components
    mainWindow = new MainWindow();
    auditService = AuditFactory::createAuditService();
    projectManager = new ProjectManager();
    updateService = new UpdateService(this);
    
    // Register as observer for updates
    updateService->addObserver(this);
    
    // Load icons from theme
    QIcon openProjectIcon = QIcon::fromTheme("folder-open");
    
    // Add toolbar buttons
    mainWindow->addToolButton(
        "Open Project",
        "Open a project directory",
        openProjectIcon,
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
    
    // Check for updates
    updateService->checkForUpdates();
    
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
    
    // UpdateService will be deleted automatically as a child QObject
    
    // Clear the singleton instance
    delete instance;
    instance = nullptr;
    
    qDebug() << "CoreTrace IDE stopped successfully";
}

void IDE::onUpdateAvailable(const QString& newVersion, const QString& downloadUrl) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Update Available");
    msgBox.setText(QString("A new version (%1) is available!").arg(newVersion));
    msgBox.setInformativeText("Would you like to download and install the update?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    
    if (msgBox.exec() == QMessageBox::Yes) {
        // Download the update
        updateService->downloadUpdate(downloadUrl);
    }
} 