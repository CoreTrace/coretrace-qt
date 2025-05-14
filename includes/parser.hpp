#pragma once

#include "audit_result.hpp"
#include <QString>
#include <QList>

class Parser {
public:
    QList<AuditResult> parse(const QString& output);
    ~Parser();
}; 