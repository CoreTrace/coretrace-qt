#include "../includes/main_window.hpp"
#include "../includes/audit_results_view.hpp"
#include <QVBoxLayout>
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QSplitter>
#include <QMenuBar>
#include <QStatusBar>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QShortcut>
#include <QKeySequence>
#include <QFileDialog>
#include <QStyle>
#include <QCheckBox>
#include <QFocusEvent>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mainSplitter(nullptr)
    , mainToolBar(nullptr)
    , centralWidget(nullptr)
    , mainLayout(nullptr)
    , fileTree(new FileTreeView(this))
    , textEditor(new QTextEdit(this))
    , fileSystemModel(new QFileSystemModel(this))
    , cliPanel(new CliOptionsPanel(this))
    , toggleCliPanelAction(new QAction("CTrace Options", this))
    , saveAction(new QAction("Save", this))
    , importAction(new QAction("Open Folder", this))
    , autosaveAction(new QAction("Autosave", this))
    , autosaveEnabled(false)
    , currentFilePath("")
{
    setupUi();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupCentralWidget();
}

MainWindow::~MainWindow()
{
    delete fileTree;
    delete textEditor;
    delete fileSystemModel;
    delete cliPanel;
    delete toggleCliPanelAction;
    delete saveAction;
    delete importAction;
    delete mainSplitter;
    delete autosaveAction;
}

void MainWindow::setupUi()
{
    setWindowTitle("CoreTrace IDE");
    resize(800, 600);
}

void MainWindow::setupMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    QMenu* fileMenu = menuBar->addMenu("File");
    
    // Change Import action to Open Folder
    importAction->setStatusTip("Open a folder in the file tree");
    connect(importAction, &QAction::triggered, this, &MainWindow::importFile);
    fileMenu->addAction(importAction);
    
    // Add Save action
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip("Save the current file");
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveCurrentFile);
    fileMenu->addAction(saveAction);
    
    // Add Autosave action
    autosaveAction->setCheckable(true);
    autosaveAction->setChecked(false);
    autosaveAction->setStatusTip("Automatically save files when editor loses focus");
    connect(autosaveAction, &QAction::triggered, this, &MainWindow::toggleAutosave);
    fileMenu->addAction(autosaveAction);
    
    // Add separator
    fileMenu->addSeparator();
    
    // Add Exit action
    QAction* exitAction = fileMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
}

void MainWindow::setupToolBar()
{
    // Create main toolbar
    mainToolBar = addToolBar("Main Toolbar");
    mainToolBar->setMovable(false);
    mainToolBar->setFloatable(false);
    
    // Keep only the CTrace Options button
    toggleCliPanelAction->setCheckable(true);
    toggleCliPanelAction->setChecked(false);
    connect(toggleCliPanelAction, &QAction::triggered, this, &MainWindow::toggleCliPanel);
    mainToolBar->addAction(toggleCliPanelAction);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready");
}

void MainWindow::setupCentralWidget()
{
    if (centralWidget) {
        delete centralWidget;
    }
    centralWidget = new QWidget(this);
    
    if (mainLayout) {
        delete mainLayout;
    }
    mainLayout = new QHBoxLayout(centralWidget);
    
    // Create main splitter
    if (mainSplitter) {
        delete mainSplitter;
    }
    mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    
    // Add file tree to the splitter
    fileTree->setMinimumWidth(150);  // Set minimum width
    fileTree->setMaximumWidth(400);  // Set maximum width
    mainSplitter->addWidget(fileTree);
    
    // Add text editor to the splitter
    mainSplitter->addWidget(textEditor);
    
    // Add CLI panel to the splitter
    cliPanel->setMinimumWidth(150);  // Set minimum width
    cliPanel->setMaximumWidth(400);  // Set maximum width
    cliPanel->setVisible(false);     // Initially hidden
    mainSplitter->addWidget(cliPanel);
    
    // Set the splitter as the main layout
    mainLayout->addWidget(mainSplitter);
    setCentralWidget(centralWidget);
    
    // Set initial splitter sizes
    QList<int> sizes;
    sizes << 250 << width() - 500 << 250;
    mainSplitter->setSizes(sizes);
    
    // Set splitter handle properties
    mainSplitter->setHandleWidth(5);  // Make the splitter handle more visible
    mainSplitter->setChildrenCollapsible(false);  // Prevent panels from collapsing completely
    
    // Connect file selection signal
    connect(fileTree, &FileTreeView::fileSelected, this, [this](const QString& filePath) {
        // Save the current file if necessary before opening a new one
        if (!currentFilePath.isEmpty() && textEditor->document()->isModified()) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Save Changes", 
                "The current file has been modified. Do you want to save changes?",
                QMessageBox::Yes|QMessageBox::No);
                
            if (reply == QMessageBox::Yes) {
                saveCurrentFile();
            }
        }
        
        // Open the new file
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            textEditor->setPlainText(in.readAll());
            file.close();
            
            // Update the current file path
            currentFilePath = filePath;
            statusBar()->showMessage("Editing: " + currentFilePath);
            
            // Reset the modification state
            textEditor->document()->setModified(false);
        }
    });

    // Connect CLI panel execute signal
    connect(cliPanel, &CliOptionsPanel::executeClicked, this, [this](const QString& options) {
        statusBar()->showMessage("Executing CTrace with options: " + options);
    });
    
    // Remove duplicate shortcut and ensure text editor captures Ctrl+S
    textEditor->setFocus();
    
    // Install an event filter to capture keyboard shortcuts
    textEditor->installEventFilter(this);
    
    // Enable document modification tracking
    connect(textEditor->document(), &QTextDocument::contentsChanged, this, [this]() {
        if (textEditor->document()->isModified() && !currentFilePath.isEmpty()) {
            setWindowTitle("CoreTrace IDE *");
            statusBar()->showMessage("Editing: " + currentFilePath + " (modified)");
        }
    });
}

// Add an eventFilter implementation to capture Ctrl+S
bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == textEditor) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            // Capture Ctrl+S
            if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_S) {
                saveCurrentFile();
                return true; // Event has been handled
            }
        }
        // Handle focus loss for autosave
        else if (event->type() == QEvent::FocusOut) {
            if (autosaveEnabled && !currentFilePath.isEmpty() && textEditor->document()->isModified()) {
                saveCurrentFile();
                // No need to return true here as we want the event to continue propagating
            }
        }
    }
    
    // Let the parent class handle other events
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::toggleCliPanel()
{
    if (!mainSplitter) return;  // Safety check
    
    bool showPanel = toggleCliPanelAction->isChecked();
    cliPanel->setVisible(showPanel);
    
    // Adjust splitter sizes
    QList<int> sizes = mainSplitter->sizes();
    int totalWidth = mainSplitter->width();
    
    if (showPanel) {
        // When showing the panel, distribute space between all three widgets
        int panelWidth = 250;  // Default width for the panel
        int remainingWidth = totalWidth - panelWidth;
        sizes[0] = qMin(250, remainingWidth / 2);  // File tree
        sizes[1] = remainingWidth - sizes[0];       // Text editor
        sizes[2] = panelWidth;                      // CLI panel
    } else {
        // When hiding the panel, distribute space between file tree and text editor
        sizes[0] = qMin(250, totalWidth / 2);      // File tree
        sizes[1] = totalWidth - sizes[0];          // Text editor
        sizes[2] = 0;                              // CLI panel
    }
    
    mainSplitter->setSizes(sizes);
}

void MainWindow::show() {
    QMainWindow::show();
}

void MainWindow::updateAuditResults(const QList<AuditResult>& results) {
    for (UIComponent* component : uiComponents) {
        if (AuditResultsView* resultsView = dynamic_cast<AuditResultsView*>(component)) {
            resultsView->displayResults(results);
        }
    }
}

void MainWindow::addToolButton(const QString& text, const QString& toolTip, 
                             const QIcon& icon, const char* slot, QObject* receiver) {
    QAction* action = new QAction(icon, text, this);
    action->setToolTip(toolTip);
    connect(action, SIGNAL(triggered()), receiver, slot);
    mainToolBar->addAction(action);
}

void MainWindow::addToolSeparator() {
    mainToolBar->addSeparator();
}

void MainWindow::addWidget(QWidget* widget, Qt::Alignment alignment) {
    mainLayout->addWidget(widget, 0, alignment);
}

void MainWindow::setProjectRoot(const QString& path) {
    fileTree->setRootPath(path);
}

void MainWindow::saveCurrentFile()
{
    if (currentFilePath.isEmpty()) {
        statusBar()->showMessage("No file to save");
        return;
    }
    
    QFile file(currentFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << textEditor->toPlainText();
        file.close();
        
        textEditor->document()->setModified(false);
        setWindowTitle("CoreTrace IDE");
        statusBar()->showMessage("File saved: " + currentFilePath);
    } else {
        QMessageBox::critical(this, "Error", "Could not save file: " + file.errorString());
        statusBar()->showMessage("Error saving file: " + file.errorString());
    }
}

void MainWindow::importFile()
{
    // Replace with folder selection instead of file
    QString folderPath = QFileDialog::getExistingDirectory(this, 
        "Open Folder", 
        "", 
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        
    if (!folderPath.isEmpty()) {
        // Use setProjectRoot method to open the folder in the tree view
        setProjectRoot(folderPath);
        statusBar()->showMessage("Opened folder: " + folderPath);
    }
}

// Implementation of toggleAutosave function
void MainWindow::toggleAutosave()
{
    autosaveEnabled = autosaveAction->isChecked();
    
    // Add a checkmark next to the menu when enabled
    if (autosaveEnabled) {
        // Use the system style checkmark icon
        autosaveAction->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
        statusBar()->showMessage("Autosave enabled", 3000);
    } else {
        // Remove the icon when disabled
        autosaveAction->setIcon(QIcon());
        statusBar()->showMessage("Autosave disabled", 3000);
    }
}