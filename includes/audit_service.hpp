#pragma once

#include "audit_result.hpp"
#include "ctrace_cli.hpp"
#include "parser.hpp"
#include "audit_cache.hpp"
#include "error_highlighter.hpp"
#include <QString>
#include <QList>

class AuditService {
public:
    AuditService();
    ~AuditService();
    
    QList<AuditResult> performAudit(const QString& file, const QString& options = "");

private:
    CTraceCLI ctraceCLI;
    Parser parser;
    AuditCache auditCache;
    ErrorHighlighter errorHighlighter;
}; 