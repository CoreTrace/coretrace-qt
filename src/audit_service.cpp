#include "../includes/audit_service.hpp"

/**
 * @class AuditService
 * @brief Provides functionality to perform audits on files.
 * 
 * The AuditService class interacts with the ctrace CLI to perform audits,
 * parses the results, caches them, and highlights errors in the files.
 */

/**
 * @brief Constructs an AuditService object.
 */
AuditService::AuditService() {}

/**
 * @brief Performs an audit on the specified file and returns the results.
 *
 * This method first checks the cache for any previously computed audit results
 * for the given file. If cached results are found, they are returned immediately.
 * Otherwise, the method executes the `ctrace` CLI tool to perform the audit,
 * parses the results, caches them for future use, and highlights any errors
 * in the file.
 *
 * @param file The path to the file to be audited.
 * @param options The options to be used with the ctrace CLI.
 * @return A QList of AuditResult objects containing the audit results.
 */
QList<AuditResult> AuditService::performAudit(const QString& file, const QString& options) {
    // Create a cache key that includes both file and options
    QString cacheKey = file + "|" + options;
    
    // Check cache first
    QList<AuditResult> cachedResults = auditCache.getCachedResults(cacheKey);
    if (!cachedResults.isEmpty()) {
        return cachedResults;
    }

    // Execute ctrace CLI with provided options
    QString output = ctraceCLI.execute(file, options);
    
    // Parse results
    QList<AuditResult> results = parser.parse(output);
    
    // Cache results with the combined key
    auditCache.cacheResults(cacheKey, results);
    
    //TODO
    // Highlight errors in the file
    //errorHighlighter.highlightErrors(file, results);
    
    return results;
}

AuditService::~AuditService() {}