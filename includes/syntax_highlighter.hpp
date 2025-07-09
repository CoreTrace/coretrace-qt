#ifndef SYNTAX_HIGHLIGHTER_HPP
#define SYNTAX_HIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>
#include <QTextDocument>

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument* parent = nullptr);
    virtual ~SyntaxHighlighter() = default;

protected:
    void highlightBlock(const QString& text) override = 0;
};

class CppHighlighter : public SyntaxHighlighter {
    Q_OBJECT

public:
    explicit CppHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    // Text formats
    QTextCharFormat keywordFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat typeFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat preprocessorFormat;

    // Multiline comment support
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    // Initialization methods
    void initKeywordRules();
    void initFunctionRules();
    void initTypeRules();
    void initCommentRules();
    void initStringRules();
    void initNumberRules();
    void initPreprocessorRules();
};

#endif // SYNTAX_HIGHLIGHTER_HPP
