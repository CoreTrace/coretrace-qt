#pragma once

#include "project.hpp"
#include <QString>
#include <QList>
#include <QObject>

class ProjectManager : public QObject {
    Q_OBJECT

public:
    ProjectManager(QObject* parent = nullptr);
    void openProject(const QString& path);
    void closeProject(Project* project);
    QList<Project*> getProjects() const;

public slots:
    void selectProjectFolder();

private:
    QList<Project*> projects;
}; 