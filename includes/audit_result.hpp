#pragma once

#include <QString>
#include <QList>

class AuditResult {
public:
    AuditResult(const QString& fileName);
    
    void addIssue(const QString& issue);
    void setLine(int lineNumber);
    void setColumn(int columnNumber);
    void setEndLine(int endLineNumber);
    void setEndColumn(int endColumnNumber);
    void setFilePath(const QString& path);
    QString getSummary() const;
    QString getFileName() const;
    QString getFilePath() const;
    QList<QString> getIssues() const;
    int getLine() const { return line; }
    int getColumn() const { return column; }
    int getEndLine() const { return endLine; }
    int getEndColumn() const { return endColumn; }
    QString getMessage() const { return message; }
    QString getRuleId() const { return ruleId; }
    void setRuleId(const QString& id);
    bool hasLocationInfo() const { return line > 0; }

private:
    QString fileName;
    QString filePath;
    QString ruleId;
    QList<QString> issues;
    int line;
    int column;
    int endLine;
    int endColumn;
    QString message;
}; 