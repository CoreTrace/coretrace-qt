#pragma once

#include <QString>
#include <QList>

class Project {
public:
    Project(const QString& name);
    
    void addFile(const QString& file);
    void removeFile(const QString& file);
    QString getName() const;
    QList<QString> getFiles() const;

private:
    QString name;
    QList<QString> files;
}; 