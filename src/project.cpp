#include "../includes/project.hpp"

/**
 * @brief Constructs a Project with the given name.
 * @param name The name of the project.
 */
Project::Project(const QString& name) : name(name) {}

/**
 * @brief Adds a file to the project if it is not already present.
 * @param file The file to add.
 */
void Project::addFile(const QString& file) {
    if (!files.contains(file)) {
        files.append(file);
    }
}

/**
 * @brief Removes a file from the project.
 * @param file The file to remove.
 */
void Project::removeFile(const QString& file) {
    files.removeAll(file);
}

/**
 * @brief Gets the name of the project.
 * @return The name of the project.
 */
QString Project::getName() const {
    return name;
}

/**
 * @brief Gets the list of files in the project.
 * @return A list of file paths.
 */
QList<QString> Project::getFiles() const {
    return files;
}