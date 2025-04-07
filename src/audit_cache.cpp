#include "../includes/audit_cache.hpp"

QList<AuditResult> AuditCache::getCachedResults(const QString& file) {
    return cache.value(file);
}

void AuditCache::cacheResults(const QString& file, const QList<AuditResult>& results) {
    cache[file] = results;
} 