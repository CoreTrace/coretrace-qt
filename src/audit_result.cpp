#include "../includes/audit_result.hpp"

AuditResult::AuditResult(const QString& fileName) : fileName(fileName) {}

void AuditResult::addIssue(const QString& issue) {
    issues.append(issue);
}

QString AuditResult::getSummary() const {
    return QString("File: %1\nIssues found: %2").arg(fileName).arg(issues.size());
}

QString AuditResult::getFileName() const {
    return fileName;
}

QList<QString> AuditResult::getIssues() const {
    return issues;
} 