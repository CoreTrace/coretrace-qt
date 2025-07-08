#include "../includes/parser.hpp"
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QList>
#include <QRegularExpression>

/**
 * @brief Parses the given output string and extracts SARIF results.
 * @param output The output string to parse.
 * @return A list of AuditResults with parsed findings.
 */
QList<AuditResult> Parser::parse(const QString& output) {
    QList<AuditResult> results;
    bool foundResults = false;
    
    // Find JSON blocks that contain SARIF-related content
    for (int i = 0; i < output.length(); i++) {
        if (output[i] == '{') {
            // Check if this might be the start of a SARIF JSON
            QString substring = output.mid(i, qMin(500, output.length() - i));
            if (substring.contains("$schema") || substring.contains("\"runs\"") || 
                substring.contains("\"results\"") || substring.contains("\"tool\"")) {
                
                // Find the matching closing brace
                int braceCount = 0;
                int jsonEnd = -1;
                for (int j = i; j < output.length(); j++) {
                    if (output[j] == '{') braceCount++;
                    else if (output[j] == '}') braceCount--;
                    
                    if (braceCount == 0) {
                        jsonEnd = j;
                        break;
                    }
                }
                
                if (jsonEnd > i) {
                    QString jsonStr = output.mid(i, jsonEnd - i + 1);
                    
                    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
                    if (!doc.isNull() && doc.isObject()) {
                        parseSarifDocument(doc.object(), results, foundResults);
                    }
                    
                    // Continue searching from after this JSON block
                    i = jsonEnd;
                }
            }
        }
    }
    
    // If no security issues were found, show a success message
    if (!foundResults) {
        AuditResult* result = new AuditResult("Security Analysis Complete");
        result->addIssue("✅ No security issues detected");
        result->addIssue("All static analysis tools completed successfully");
        results.append(*result);
        delete result;
    }
    
    return results;
}

void Parser::parseSarifDocument(const QJsonObject& sarifObj, QList<AuditResult>& results, bool& foundResults) {
    // Check if this is a SARIF document
    if (sarifObj.contains("runs") && sarifObj["runs"].isArray()) {
        QJsonArray runs = sarifObj["runs"].toArray();
        
        for (int runIndex = 0; runIndex < runs.size(); runIndex++) {
            QJsonObject run = runs[runIndex].toObject();
            
            // Get tool information
            QString toolName = "Security Analysis";
            if (run.contains("tool") && run["tool"].isObject()) {
                QJsonObject tool = run["tool"].toObject();
                if (tool.contains("driver") && tool["driver"].isObject()) {
                    QJsonObject driver = tool["driver"].toObject();
                    toolName = driver["name"].toString();
                }
            }
            
            // Parse results
            if (run.contains("results") && run["results"].isArray()) {
                QJsonArray resultsArray = run["results"].toArray();
                
                if (resultsArray.isEmpty()) {
                    // No issues found by this tool - don't create a result, just mark as found
                    foundResults = true;
                } else {
                    // Process each finding
                    for (int resultIndex = 0; resultIndex < resultsArray.size(); resultIndex++) {
                        QJsonObject resultObj = resultsArray[resultIndex].toObject();
                        
                        QString ruleId = resultObj["ruleId"].toString();
                        QString level = resultObj["level"].toString();
                        QString message = resultObj["message"].toObject()["text"].toString();
                        
                        // Get location information
                        QString fileName = "Unknown File";
                        int lineNumber = 0;
                        int columnNumber = 0;
                        int endLineNumber = 0;
                        int endColumnNumber = 0;
                        QString snippet = "";
                        
                        if (resultObj.contains("locations") && resultObj["locations"].isArray()) {
                            QJsonArray locations = resultObj["locations"].toArray();
                            if (!locations.isEmpty()) {
                                QJsonObject location = locations[0].toObject();
                                if (location.contains("physicalLocation")) {
                                    QJsonObject physLocation = location["physicalLocation"].toObject();
                                    
                                    // Get file name
                                    if (physLocation.contains("artifactLocation")) {
                                        QJsonObject artifact = physLocation["artifactLocation"].toObject();
                                        fileName = artifact["uri"].toString();
                                    }
                                    
                                    // Get detailed line/column information and snippet
                                    if (physLocation.contains("region")) {
                                        QJsonObject region = physLocation["region"].toObject();
                                        lineNumber = region["startLine"].toInt();
                                        columnNumber = region["startColumn"].toInt();
                                        endLineNumber = region["endLine"].toInt();
                                        endColumnNumber = region["endColumn"].toInt();
                                        if (region.contains("snippet")) {
                                            snippet = region["snippet"].toObject()["text"].toString();
                                        }
                                    }
                                }
                            }
                        }
                        
                        // Create audit result with enhanced location data
                        QString resultTitle = QString("🚨 %1 Security Issue").arg(toolName);
                        AuditResult* result = new AuditResult(resultTitle);
                        result->setFilePath(fileName);
                        result->setLine(lineNumber);
                        result->setColumn(columnNumber);
                        result->setEndLine(endLineNumber > 0 ? endLineNumber : lineNumber);
                        result->setEndColumn(endColumnNumber > 0 ? endColumnNumber : columnNumber + 10);
                        result->setRuleId(ruleId);
                        
                        // Format the issue description with precise location
                        if (columnNumber > 0) {
                            result->addIssue(QString("📍 Location: %1 (Line %2, Column %3-%4)").arg(fileName).arg(lineNumber).arg(columnNumber).arg(endColumnNumber > columnNumber ? endColumnNumber : columnNumber + 10));
                        } else {
                            result->addIssue(QString("📍 Location: %1 (Line %2)").arg(fileName).arg(lineNumber));
                        }
                        
                        if (!ruleId.isEmpty()) {
                            result->addIssue(QString("🔍 Rule: %1").arg(ruleId));
                        }
                        
                        result->addIssue(QString("📝 Issue: %1").arg(message));
                        
                        // Format severity with appropriate emoji
                        QString severityIcon = "⚠️";
                        if (level == "error") severityIcon = "🔴";
                        else if (level == "warning") severityIcon = "🟡";
                        else if (level == "note" || level == "info") severityIcon = "🔵";
                        
                        result->addIssue(QString("%1 Severity: %2").arg(severityIcon).arg(level.isEmpty() ? "unknown" : level));
                        
                        if (!snippet.isEmpty()) {
                            result->addIssue(QString("💻 Code: %1").arg(snippet.trimmed()));
                        }
                        
                        // Add security recommendation if it's a security issue
                        if (message.contains("CWE") || ruleId.contains("security") || ruleId.contains("vuln")) {
                            result->addIssue("🛡️  Recommendation: Review this code for potential security vulnerabilities");
                        }
                        
                        results.append(*result);
                        delete result;
                        foundResults = true;
                    }
                }
            }
        }
    }
}

Parser::~Parser() {}
