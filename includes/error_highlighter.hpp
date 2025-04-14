#pragma once

#include "audit_result.hpp"
#include <QString>
#include <QList>

class ErrorHighlighter {
public:
    void highlightErrors(const QString& file, const QList<AuditResult>& results);
}; 