#include "../includes/ide.hpp"
#include "../includes/audit_factory.hpp"
#include <QDebug>
#include <QIcon>
#include <QDir>

/**
 * @class IDE
 * @brief The main controller for the CoreTrace IDE.
 * 
 * The IDE class manages the main window, project manager, and audit service.
 * It serves as the central point for initializing and shutting down the IDE.
 */

IDE* IDE::instance = nullptr;

/**
 * @brief Retrieves the singleton instance of the IDE.
 * 
 * Ensures that only one instance of the IDE exists throughout the application.
 * 
 * @return A pointer to the IDE instance.
 */
IDE* IDE::getInstance() {
    if (!instance) {
        instance = new IDE();
    }
    return instance;
}

/**
 * @brief Constructs the IDE object.
 * 
 * Initializes the main window, project manager, and audit service.
 * Sets up the toolbar buttons and prepares the IDE for use.
 */
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

/**
 * @brief Destroys the IDE object.
 * 
 * Cleans up resources and components. The actual cleanup is handled by the `stop()` method.
 */
IDE::~IDE() {
    // Cleanup is handled by stop()
}

/**
 * @brief Starts the CoreTrace IDE.
 * 
 * Displays the main window and initializes any saved project state or settings.
 */
void IDE::start() {
    qDebug() << "Starting CoreTrace IDE...";
    
    // Initialize the main window
    mainWindow->show();
    
    // TODO: Load any saved project state or settings
    
    qDebug() << "CoreTrace IDE started successfully";
}

/**
 * @brief Stops the CoreTrace IDE.
 * 
 * Closes all open projects, cleans up resources, and resets the singleton instance.
 */
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