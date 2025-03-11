#pragma once
#include <QMenuBar>
#include <QTextEdit>
#include <QWidget>
#include <QMessageBox>
#include <QStackedWidget>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QApplication>

class Board : public QWidget {
    Q_OBJECT

public:
    ~Board();
    Board(QWidget *parent = nullptr);
    int SaveFile();
    int OpenFile();

protected:
    void keyPressEvent(QKeyEvent *event) override;

    QTextEdit *user_input = new QTextEdit(this);
    QMenuBar *menuBar = new QMenuBar(this);

private slots:
    void showShortcuts();
    void showAbout();
};
