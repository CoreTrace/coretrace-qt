#include "../includes/syntax_highlighter.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {}

CppHighlighter::CppHighlighter(QTextDocument* parent)
    : SyntaxHighlighter(parent)
{
    keywordFormat.setForeground(QColor(86, 156, 214));  // Blue
    keywordFormat.setFontWeight(QFont::Bold);

    functionFormat.setForeground(QColor(220, 220, 170));  // Light yellow
    functionFormat.setFontWeight(QFont::Bold);

    typeFormat.setForeground(QColor(78, 201, 176));  // Teal
    typeFormat.setFontWeight(QFont::Bold);

    commentFormat.setForeground(QColor(106, 153, 85));  // Green

    stringFormat.setForeground(QColor(206, 145, 120));  // Orange

    numberFormat.setForeground(QColor(181, 206, 168));  // Light green

    preprocessorFormat.setForeground(QColor(155, 155, 255));  // Light blue
    preprocessorFormat.setFontWeight(QFont::Bold);

    // Initialize all rules
    initKeywordRules();
    initFunctionRules();
    initTypeRules();
    initCommentRules();
    initStringRules();
    initNumberRules();
    initPreprocessorRules();
}

void CppHighlighter::initKeywordRules() {
    QStringList keywords = {
        "char", "class", "const", "double", "enum", "explicit", "friend",
        "inline", "int", "long", "namespace", "operator", "private", "protected",
        "public", "short", "signals", "signed", "slots", "static", "struct",
        "template", "typedef", "typename", "union", "unsigned", "virtual", "void",
        "volatile", "return", "if", "else", "for", "while", "do", "switch", "case",
        "break", "continue", "goto", "default", "throw", "try", "catch", "new",
        "delete", "this", "true", "false", "nullptr"
    };

    for (const QString& word : keywords) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("\\b" + word + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
}

void CppHighlighter::initFunctionRules() {
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\s*\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);
}

void CppHighlighter::initTypeRules() {
    QStringList types = {
        "int", "char", "double", "float", "long", "short", "void", "bool",
        "string", "vector", "list", "map", "set", "deque", "stack", "queue",
        "priority_queue"
    };

    for (const QString& word : types) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("\\b" + word + "\\b");
        rule.format = typeFormat;
        highlightingRules.append(rule);
    }
}

void CppHighlighter::initCommentRules() {
    // Single-line comments
    HighlightingRule rule;
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = commentFormat;
    highlightingRules.append(rule);

    // Multiline comment expressions
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void CppHighlighter::initStringRules() {
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\"([^\"\\\\]|\\\\.)*\"");
    rule.format = stringFormat;
    highlightingRules.append(rule);
}

void CppHighlighter::initNumberRules() {
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\b\\d+\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);
}

void CppHighlighter::initPreprocessorRules() {
    HighlightingRule rule;
    rule.pattern = QRegularExpression("^\\s*#\\s*\\w+");
    rule.format = preprocessorFormat;
    highlightingRules.append(rule);
}

void CppHighlighter::highlightBlock(const QString& text) {
    for (const HighlightingRule& rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Multiline comments
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch = commentEndExpression.match(text, startIndex);
        int endIndex = endMatch.hasMatch() ? endMatch.capturedEnd() : -1;
        int commentLength = (endIndex == -1) ? text.length() - startIndex : endIndex - startIndex;

        setFormat(startIndex, commentLength, commentFormat);
        if (endIndex == -1) {
            setCurrentBlockState(1);
            break;
        }

        startIndex = text.indexOf(commentStartExpression, endIndex);
    }
}
