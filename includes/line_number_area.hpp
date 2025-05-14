#ifndef LINE_NUMBER_AREA_HPP
#define LINE_NUMBER_AREA_HPP

#include <QWidget>
#include <QTextEdit>

class LineNumberArea : public QWidget {
public:
    LineNumberArea(QTextEdit* editor);

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

private:
    QTextEdit* textEditor;
};

#endif // LINE_NUMBER_AREA_HPP 