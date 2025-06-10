#pragma once

#include "audit_result.hpp"
#include <QString>
#include <QList>
#include <QStringList>

class Parser {
public:
    QList<AuditResult> parse(const QString& output);
    ~Parser();
}; 