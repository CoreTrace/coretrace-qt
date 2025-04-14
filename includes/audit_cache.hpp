#pragma once

#include "audit_result.hpp"
#include <QString>
#include <QList>
#include <QMap>

class AuditCache {
public:
    QList<AuditResult> getCachedResults(const QString& file);
    void cacheResults(const QString& file, const QList<AuditResult>& results);

private:
    QMap<QString, QList<AuditResult>> cache;
}; 