#include "../includes/parser.hpp"
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QList>

/**
 * @brief Parses the given output string and extracts type and description from JSON.
 * @param output The output string to parse.
 * @return A list containing a single AuditResult with the formatted output.
 */
QList<AuditResult> Parser::parse(const QString& output) {
    QList<AuditResult> results;
    
    // Create a single audit result
    AuditResult* result = new AuditResult("Command Output");
    
    // Split the output into command and JSON parts
    QStringList parts = output.split('\n', Qt::SkipEmptyParts);
    if (parts.size() >= 2) {
        // First line is the command
        result->addIssue("Command: " + parts[0].trimmed());
        
        // Try to parse the JSON part
        QString jsonStr = parts[1].trimmed();
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject obj = doc.object();
            QString type = obj["type"].toString();
            QString description = obj["description"].toString();
            
            result->addIssue("\nType: " + type);
            result->addIssue("Description: " + description);
        } else {
            // If JSON parsing fails, show the raw output
            result->addIssue("\nRaw output:\n" + output);
        }
    } else {
        // If we can't split the output, show it as is
        result->addIssue(output);
    }
    
    results.append(*result);
    delete result;
    
    return results;
}

Parser::~Parser() {}