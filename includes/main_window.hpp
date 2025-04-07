#pragma once

#include "ui_component.hpp"
#include "audit_result.hpp"
#include "file_tree_view.hpp"
#include <QMainWindow>
#include <QList>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTextEdit>
#include <QFileSystemModel>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void show();
    void updateAuditResults(const QList<AuditResult>& results);
    void setProjectRoot(const QString& path);
    
    // Widget management
    void addToolButton(const QString& text, const QString& toolTip, const QIcon& icon, 
                      const char* slot, QObject* receiver);
    void addToolSeparator();
    void addWidget(QWidget* widget, Qt::Alignment alignment = Qt::AlignTop);

    FileTreeView* getFileTreeView() const { return fileTree; }
    QTextEdit* getTextEditor() const { return textEditor; }

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupCentralWidget();

    QList<UIComponent*> uiComponents;
    QToolBar* mainToolBar;
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    FileTreeView* fileTree;
    QSplitter* mainSplitter;
    QTextEdit* textEditor;
    QFileSystemModel* fileSystemModel;
}; 