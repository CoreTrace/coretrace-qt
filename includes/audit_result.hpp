#pragma once

#include <QString>
#include <QList>

class AuditResult {
public:
    AuditResult(const QString& fileName);
    
    void addIssue(const QString& issue);
    QString getSummary() const;
    QString getFileName() const;
    QList<QString> getIssues() const;

private:
    QString fileName;
    QList<QString> issues;
}; 