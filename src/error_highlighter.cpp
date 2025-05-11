#include "../includes/error_highlighter.hpp"
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QFile>
#include <QTextStream>

/**
 * @brief Highlights errors in the specified file based on audit results.
 * @param file The file to highlight errors in.
 * @param results The list of audit results containing error information.
 */
void ErrorHighlighter::highlightErrors(const QString& file, const QList<AuditResult>& results) {
    QFile sourceFile(file);
    if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&sourceFile);
    QString content = in.readAll();
    sourceFile.close();

    QTextDocument document;
    document.setPlainText(content);

    QTextCharFormat errorFormat;
    errorFormat.setBackground(Qt::red);
    errorFormat.setForeground(Qt::white);

    for (const AuditResult& result : results) {
        if (result.getFileName() == file) {
            for (const QString& issue : result.getIssues()) {
                // This is a simplified version - in a real implementation,
                // you would parse the issue string to get line numbers
                // and highlight specific lines
                QTextCursor cursor(&document);
                cursor.movePosition(QTextCursor::Start);
                
                while (!cursor.atEnd()) {
                    cursor.movePosition(QTextCursor::NextWord);
                    if (cursor.selectedText().contains(issue)) {
                        cursor.mergeCharFormat(errorFormat);
                    }
                }
            }
        }
    }
}