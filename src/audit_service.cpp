#include "../includes/audit_service.hpp"

AuditService::AuditService() {}

QList<AuditResult> AuditService::performAudit(const QString& file) {
    // Check cache first
    QList<AuditResult> cachedResults = auditCache.getCachedResults(file);
    if (!cachedResults.isEmpty()) {
        return cachedResults;
    }

    // Execute ctrace CLI
    QString output = ctraceCLI.execute(file, "--all");
    
    // Parse results
    QList<AuditResult> results = parser.parse(output);
    
    // Cache results
    auditCache.cacheResults(file, results);
    
    // Highlight errors in the file
    errorHighlighter.highlightErrors(file, results);
    
    return results;
} 