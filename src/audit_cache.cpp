#include "../includes/audit_cache.hpp"

/**
 * @brief Retrieves cached audit results for a given file and options combination.
 * @param cacheKey The combined file path and options string.
 * @return A list of cached audit results, or an empty list if no results are found.
 */
QList<AuditResult> AuditCache::getCachedResults(const QString& cacheKey) {
    return cache.value(cacheKey);
}

/**
 * @brief Caches the audit results for a specified file and options combination.
 *
 * This function stores the provided list of audit results associated with
 * the given cache key (file path + options) in the cache. If the key already exists
 * in the cache, its associated results will be overwritten.
 *
 * @param cacheKey The combined file path and options string.
 * @param results A QList of AuditResult objects representing the audit results to be cached.
 */
void AuditCache::cacheResults(const QString& cacheKey, const QList<AuditResult>& results) {
    cache[cacheKey] = results;
} 