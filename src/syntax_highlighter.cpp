#include "../includes/syntax_highlighter.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
}

CppHighlighter::CppHighlighter(QTextDocument* parent)
    : SyntaxHighlighter(parent)
{
    // Set up keyword format
    keywordFormat.setForeground(QColor(86, 156, 214));  // Blue
    keywordFormat.setFontWeight(QFont::Bold);

    // Set up function format
    functionFormat.setForeground(QColor(220, 220, 170));  // Light yellow
    functionFormat.setFontWeight(QFont::Bold);

    // Set up type format
    typeFormat.setForeground(QColor(78, 201, 176));  // Teal
    typeFormat.setFontWeight(QFont::Bold);

    // Set up comment format
    commentFormat.setForeground(QColor(106, 153, 85));  // Green

    // Set up string format
    stringFormat.setForeground(QColor(206, 145, 120));  // Orange

    // Set up number format
    numberFormat.setForeground(QColor(181, 206, 168));  // Light green

    // C++ keywords
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                   << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                   << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                   << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                   << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                   << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                   << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                   << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                   << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                   << "\\bvoid\\b" << "\\bvolatile\\b" << "\\breturn\\b"
                   << "\\bif\\b" << "\\belse\\b" << "\\bfor\\b" << "\\bwhile\\b"
                   << "\\bdo\\b" << "\\bswitch\\b" << "\\bcase\\b" << "\\bbreak\\b"
                   << "\\bcontinue\\b" << "\\bgoto\\b" << "\\bdefault\\b"
                   << "\\bthrow\\b" << "\\btry\\b" << "\\bcatch\\b"
                   << "\\bnew\\b" << "\\bdelete\\b" << "\\bthis\\b"
                   << "\\btrue\\b" << "\\bfalse\\b" << "\\bnullptr\\b";

    for (const QString& pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Function names
    HighlightingRule functionRule;
    functionRule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\s*\\()");
    functionRule.format = functionFormat;
    highlightingRules.append(functionRule);

    // Types
    QStringList typePatterns;
    typePatterns << "\\bint\\b" << "\\bchar\\b" << "\\bdouble\\b"
                << "\\bfloat\\b" << "\\blong\\b" << "\\bshort\\b"
                << "\\bvoid\\b" << "\\bbool\\b" << "\\bstring\\b"
                << "\\bvector\\b" << "\\blist\\b" << "\\bmap\\b"
                << "\\bset\\b" << "\\bdeque\\b" << "\\bstack\\b"
                << "\\bqueue\\b" << "\\bpriority_queue\\b";

    for (const QString& pattern : typePatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = typeFormat;
        highlightingRules.append(rule);
    }

    // Comments
    HighlightingRule commentRule;
    commentRule.pattern = QRegularExpression("//[^\n]*");
    commentRule.format = commentFormat;
    highlightingRules.append(commentRule);

    // Strings
    HighlightingRule stringRule;
    stringRule.pattern = QRegularExpression("\".*\"");
    stringRule.format = stringFormat;
    highlightingRules.append(stringRule);

    // Numbers
    HighlightingRule numberRule;
    numberRule.pattern = QRegularExpression("\\b\\d+\\b");
    numberRule.format = numberFormat;
    highlightingRules.append(numberRule);
}

void CppHighlighter::highlightBlock(const QString& text)
{
    for (const HighlightingRule& rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
} 