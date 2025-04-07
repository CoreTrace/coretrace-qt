#include "../includes/project.hpp"

Project::Project(const QString& name) : name(name) {}

void Project::addFile(const QString& file) {
    if (!files.contains(file)) {
        files.append(file);
    }
}

void Project::removeFile(const QString& file) {
    files.removeAll(file);
}

QString Project::getName() const {
    return name;
}

QList<QString> Project::getFiles() const {
    return files;
} 