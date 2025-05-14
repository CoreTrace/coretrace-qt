#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>

class OutputDisplay : public QWidget {
    Q_OBJECT

public:
    explicit OutputDisplay(QWidget* parent = nullptr);
    void setOutput(const QString& output);
    void clear();

private:
    QTextEdit* outputText;
    QLabel* titleLabel;
    
    void setupUi();
}; 