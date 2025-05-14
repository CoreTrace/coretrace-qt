#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "ui_component.hpp"
#include "audit_result.hpp"
#include "file_tree_view.hpp"
#include "cli_options_panel.hpp"
#include "output_display.hpp"
#include <QMainWindow>
#include <QList>
#include <QToolBar>
#include <QAction>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTextEdit>
#include <QFileSystemModel>
#include <QKeyEvent>
#include <QVBoxLayout>

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
    void addWidget(QWidget* widget, Qt::Alignment alignment = Qt::Alignment());

    FileTreeView* getFileTreeView() const { return fileTree; }
    QTextEdit* getTextEditor() const { return textEditor; }

public slots:
    void toggleCliPanel();
    void saveCurrentFile();
    void importFile(); 
    void toggleAutosave();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupCentralWidget();

    // Member variables in initialization order
    QSplitter* mainSplitter;
    QToolBar* mainToolBar;
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    FileTreeView* fileTree;
    QTextEdit* textEditor;
    QFileSystemModel* fileSystemModel;
    CliOptionsPanel* cliPanel;
    OutputDisplay* outputDisplay;
    QAction* toggleCliPanelAction;
    QAction* saveAction;
    QAction* importAction;
    QAction* autosaveAction;
    bool autosaveEnabled;
    QString currentFilePath;
    QList<UIComponent*> uiComponents;
};

#endif // MAIN_WINDOW_HPP