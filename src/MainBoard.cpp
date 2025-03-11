#include "../includes/MainBoard.hpp"

Board::~Board() {
    delete this->user_input;
    delete this->menuBar;
}

Board::Board(QWidget *parent) : QWidget(parent), user_input(new QTextEdit(this)), menuBar(new QMenuBar(this)) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    this->user_input->setPlaceholderText(QStringLiteral("Start coding !"));
    layout->addWidget(this->user_input);

    QMenu *helpMenu = menuBar->addMenu(QStringLiteral("Help"));
    QAction *shortcutsAction = helpMenu->addAction(QStringLiteral("Shortcuts"));
    QAction *shortcutsAbout = helpMenu->addAction(QStringLiteral("About"));
    connect(shortcutsAbout, &QAction::triggered, this, &Board::showAbout);
    connect(shortcutsAction, &QAction::triggered, this, &Board::showShortcuts);

    layout->setMenuBar(menuBar);
    setLayout(layout);

    this->setStyleSheet("background-color: #282A36; color: #F8F8F2;");
    menuBar->setStyleSheet("QMenuBar { background-color: #282A36; color: #F8F8F2; }"
                           "QMenuBar::item { background-color: #282A36; color: #F8F8F2; }"
                           "QMenuBar::item:selected { background-color: #44475a; }"
                           "QMenu { background-color: #282A36; color: #F8F8F2; }"
                           "QMenu::item { background-color: #282A36; color: #F8F8F2; }"
                           "QMenu::item:selected { background-color: #44475a; }");
}

int Board::SaveFile() {
    QString fileName = QFileDialog::getSaveFileName(this, QStringLiteral("Save File"), QStringLiteral("./"), QStringLiteral("Text Files (*)"));
        if (fileName.isEmpty()) {
            QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("A file name is required"));
            return 1;
        }
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Could not save file"));
            return 1;
        }
        QTextStream out(&file);
        out << this->user_input->toPlainText();
        file.close();
        QMessageBox::information(this, QStringLiteral("Save"), QStringLiteral("File saved !"));
    return 0;
}

int Board::OpenFile() {
    QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Open File"), QStringLiteral("./"), QStringLiteral("Any file (*)"));
        if (fileName.isEmpty()) {
            QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Could not open file"));
            return 1;
        }
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Could not open file"));
            return 1;
        }
        QTextStream in(&file);
        this->user_input->setPlainText(in.readAll());
        file.close();
        QMessageBox::information(this, QStringLiteral("Open"), QStringLiteral("File opened !"));
    return 0;
}

void Board::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_S && event->modifiers() == Qt::ControlModifier) {
        this->SaveFile();
    } else if (event->key() == Qt::Key_O && event->modifiers() == Qt::ControlModifier) {
        this->OpenFile();
    } else if (event->key() == Qt::Key_Z && event->modifiers() == Qt::ControlModifier) {
        QMessageBox::information(this, QStringLiteral("Undo"), QStringLiteral("Undo action !"));
    } else if (event->key() == Qt::Key_Y && event->modifiers() == Qt::ControlModifier) {
        QMessageBox::information(this, QStringLiteral("Redo"), QStringLiteral("Redo action !"));
    } else if (event->key() == Qt::Key_Escape) {
        exit(0);
    } else {
        QWidget::keyPressEvent(event);
    }
}

void Board::showShortcuts() {
    QMessageBox::information(this, QStringLiteral("Shortcuts"), QStringLiteral("List of shortcuts:\n\n"
        "Escape: Exit the application\n"
        "Ctrl+S: Save\n"
        "Ctrl+O: Open\n"
        "Ctrl+Z: Undo\n"
        "Ctrl+Y: Redo"));
}

void Board::showAbout() {
    QMessageBox::information(this, QStringLiteral("About"), QStringLiteral("This is a simple text editor made with Qt\n Author: shookapic (github.com/shookapic)"));
}
