#include "../includes/line_number_area.hpp"
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>

LineNumberArea::LineNumberArea(QTextEdit* editor) : QWidget(editor), textEditor(editor) {
    setFixedWidth(40);  // Fixed width for line numbers
}

void LineNumberArea::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    
    // Use system palette colors
    QPalette pal = palette();
    painter.fillRect(event->rect(), pal.base());  // Background color
    painter.setPen(pal.text().color());  // Text color

    QTextDocument* doc = textEditor->document();
    int blockCount = doc->blockCount();
    int lineHeight = textEditor->fontMetrics().height();
    int visibleLines = height() / lineHeight;
    int firstVisibleLine = textEditor->verticalScrollBar()->value() / lineHeight;

    for (int i = 0; i < visibleLines && (firstVisibleLine + i) < blockCount; ++i) {
        int lineNumber = firstVisibleLine + i + 1;
        int y = i * lineHeight;
        painter.drawText(0, y, width() - 5, lineHeight,
                        Qt::AlignRight, QString::number(lineNumber));
    }
}

QSize LineNumberArea::sizeHint() const {
    return QSize(40, 0);  // Fixed width of 40 pixels
} 