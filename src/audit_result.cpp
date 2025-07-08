#include "../includes/audit_result.hpp"

/**
 * @class AuditResult
 * @brief Represents the results of an audit for a specific file.
 * 
 * The AuditResult class stores the file name and a list of issues
 * identified during the audit process. It provides methods to add
 * issues and retrieve a summary of the results.
 */

/**
 * @brief Constructs an AuditResult object for a specific file.
 * 
 * @param fileName The name of the file being audited.
 */
AuditResult::AuditResult(const QString& fileName) : fileName(fileName), line(0), column(0), endLine(0), endColumn(0) {}

/**
 * @brief Adds an issue to the audit result.
 * 
 * @param issue A description of the issue identified during the audit.
 */
void AuditResult::addIssue(const QString& issue) {
    issues.append(issue);
    // Set the message to be the concatenation of all issues
    message = issues.join("\n");
}

/**
 * @brief Sets the line number for this audit result.
 * 
 * @param lineNumber The line number where the issue was found.
 */
void AuditResult::setLine(int lineNumber) {
    line = lineNumber;
}

/**
 * @brief Sets the column number for this audit result.
 * 
 * @param columnNumber The column number where the issue was found.
 */
void AuditResult::setColumn(int columnNumber) {
    column = columnNumber;
}

/**
 * @brief Sets the end line number for this audit result.
 * 
 * @param endLineNumber The end line number of the issue.
 */
void AuditResult::setEndLine(int endLineNumber) {
    endLine = endLineNumber;
}

/**
 * @brief Sets the end column number for this audit result.
 * 
 * @param endColumnNumber The end column number of the issue.
 */
void AuditResult::setEndColumn(int endColumnNumber) {
    endColumn = endColumnNumber;
}

/**
 * @brief Sets the file path for this audit result.
 * 
 * @param path The full file path.
 */
void AuditResult::setFilePath(const QString& path) {
    filePath = path;
}

/**
 * @brief Sets the rule ID for this audit result.
 * 
 * @param id The rule ID that triggered this result.
 */
void AuditResult::setRuleId(const QString& id) {
    ruleId = id;
}

/**
 * @brief Generates a summary of the audit result.
 * 
 * This method constructs a summary string containing the file name
 * and the number of issues found during the audit.
 * 
 * @return A QString containing the summary in the format:
 *         "File: <fileName>\nIssues found: <number_of_issues>"
 */
QString AuditResult::getSummary() const {
    return QString("File: %1\nIssues found: %2").arg(fileName).arg(issues.size());
}

/**
 * @brief Retrieves the name of the file associated with this audit result.
 * 
 * @return The file name as a QString.
 */
QString AuditResult::getFileName() const {
    return fileName;
}

/**
 * @brief Retrieves the full file path associated with this audit result.
 * 
 * @return The file path as a QString.
 */
QString AuditResult::getFilePath() const {
    return filePath;
}

/**
 * @brief Retrieves the list of issues identified during the audit.
 * 
 * @return A QList of QStrings, each representing an issue.
 */
QList<QString> AuditResult::getIssues() const {
    return issues;
}