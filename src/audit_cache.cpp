#include "../includes/audit_cache.hpp"

QList<AuditResult> AuditCache::getCachedResults(const QString& file) {
    return cache.value(file);
}

/**
 * @brief Caches the audit results for a specified file.
 *
 * This function stores the provided list of audit results associated with
 * the given file in the cache. If the file already exists in the cache,
 * its associated results will be overwritten.
 *
 * @param file The file path as a QString for which the audit results are being cached.
 * @param results A QList of AuditResult objects representing the audit results to be cached.
 */
void AuditCache::cacheResults(const QString& file, const QList<AuditResult>& results) {
    cache[file] = results;
} 