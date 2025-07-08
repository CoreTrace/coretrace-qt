#pragma once

#include "audit_result.hpp"
#include <QString>
#include <QList>
#include <QJsonObject>

class Parser {
public:
    QList<AuditResult> parse(const QString& output);
    ~Parser();

private:
    void parseSarifDocument(const QJsonObject& sarifObj, QList<AuditResult>& results, bool& foundResults);
}; 