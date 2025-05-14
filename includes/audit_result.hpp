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
    int getLine() const { return line; }
    QString getMessage() const { return message; }

private:
    QString fileName;
    QList<QString> issues;
    int line;
    QString message;
}; 