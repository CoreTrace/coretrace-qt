#include "../includes/parser.hpp"
#include <QStringList>

//TODO 
QList<AuditResult> Parser::parse(const QString& output) {
    return QList<AuditResult>();
    // QList<AuditResult> results;
    // QStringList lines = output.split('\n');
    
    // QString currentFile;
    // AuditResult* currentResult = nullptr;
    
    // for (const QString& line : lines) {
    //     if (line.startsWith("File: ")) {
    //         if (currentResult) {
    //             results.append(*currentResult);
    //             delete currentResult;
    //         }
    //         currentFile = line.mid(6).trimmed();
    //         currentResult = new AuditResult(currentFile);
    //     } else if (currentResult && !line.trimmed().isEmpty()) {
    //         currentResult->addIssue(line.trimmed());
    //     }
    // }
    
    // if (currentResult) {
    //     results.append(*currentResult);
    //     delete currentResult;
    // }
    
    // return results;
}
