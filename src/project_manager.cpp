#include "../includes/project_manager.hpp"
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include "../includes/ide.hpp"

ProjectManager::ProjectManager(QObject* parent) : QObject(parent) {}

void ProjectManager::openProject(const QString& path) {
    QFileInfo fileInfo(path);
    if (!fileInfo.exists() || !fileInfo.isDir()) {
        return;
    }

    Project* project = new Project(fileInfo.fileName());
    QDir dir(path);
    
    // Add all files in the directory
    QStringList files = dir.entryList(QDir::Files);
    for (const QString& file : files) {
        project->addFile(dir.filePath(file));
    }
    
    projects.append(project);

    // Update the file tree
    IDE::getInstance()->getMainWindow()->setProjectRoot(path);
}

void ProjectManager::closeProject(Project* project) {
    if (projects.contains(project)) {
        projects.removeAll(project);
        delete project;
    }
}

QList<Project*> ProjectManager::getProjects() const {
    return projects;
}

void ProjectManager::selectProjectFolder() {
    QString folderPath = QFileDialog::getExistingDirectory(
        nullptr,
        "Select Project Folder",
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!folderPath.isEmpty()) {
        openProject(folderPath);
        QMessageBox::information(nullptr, "Project Opened", 
            QString("Project opened successfully: %1").arg(folderPath));
    }
} 