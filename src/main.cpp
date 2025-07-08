#include "../includes/ide.hpp"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include <iostream>

/**
 * @brief Checks for application updates.
 */
void CheckForUpdates() {
    // TODO: Implement update check
}

/**
 * @brief Checks if the required icons are present in the application directory.
 */
void CheckRequiredIcons() {
    QDir iconsDir("./icons");
    if (!iconsDir.exists()) {
        QMessageBox::critical(nullptr, "Error", "Icons directory not found.");
        std::cerr << "Icons directory not found." << std::endl;
        exit(1);
        return;
    }

    QStringList requiredIcons = {"coretrace.png"};
    QStringList missingIcons;

    for (const QString& icon : requiredIcons) {
        if (!iconsDir.exists(icon)) {
            missingIcons.append(icon);
        }
    }

    if (!missingIcons.isEmpty()) {
        QMessageBox::critical(nullptr, "Error", 
            "Missing required icons: " + missingIcons.join(", "));
        std::cerr << "Missing required icons: " + missingIcons.join(", ").toStdString() << std::endl;
        exit(1);
    }
}

/**
 * @brief Checks if all application requirements are met.
 */
void CheckRequirements() {
    QString currentPath = QDir::currentPath();
    std::cout << "Current working directory: " << currentPath.toStdString() << std::endl;
    
    QString cliPath = QDir::currentPath() + "/coretrace/build/ctrace";
    if (!QFile::exists(cliPath)) {
        QMessageBox::critical(nullptr, "Error", 
            "Coretrace not found\n"
            "Expected path: " + cliPath);
        std::cerr << "coretrace-cli not found in the current directory." << std::endl;
        std::cerr << "Expected path: " << cliPath.toStdString() << std::endl;
        exit(1);
    }
    CheckRequiredIcons();
}

/**
 * @brief The main entry point of the application.
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return The exit code of the application.
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CheckForUpdates();
    CheckRequirements();
    
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
