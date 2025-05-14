#include "../includes/output_display.hpp"
#include <QFont>
#include <QPalette>

OutputDisplay::OutputDisplay(QWidget* parent)
    : QWidget(parent)
    , outputText(new QTextEdit(this))
    , titleLabel(new QLabel("Command Output", this))
{
    setupUi();
}

void OutputDisplay::setupUi()
{
    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);

    // Style the title
    titleLabel->setStyleSheet(
        "QLabel {"
        "   color: #2c3e50;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   padding: 5px;"
        "   border-bottom: 2px solid #3498db;"
        "}"
    );

    // Style the output text
    outputText->setReadOnly(true);
    outputText->setStyleSheet(
        "QTextEdit {"
        "   background-color: #f8f9fa;"
        "   border: 1px solid #dee2e6;"
        "   border-radius: 4px;"
        "   padding: 8px;"
        "   font-family: 'Consolas', 'Monaco', monospace;"
        "   font-size: 12px;"
        "   color: #2c3e50;"
        "}"
        "QTextEdit:focus {"
        "   border: 1px solid #3498db;"
        "}"
    );

    // Set minimum size
    setMinimumWidth(300);
    outputText->setMinimumHeight(200);

    // Add widgets to layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(outputText);
}

void OutputDisplay::setOutput(const QString& output)
{
    outputText->setPlainText(output);
    // Scroll to the top
    outputText->moveCursor(QTextCursor::Start);
}

void OutputDisplay::clear()
{
    outputText->clear();
} 