#include "../includes/main_window.hpp"
#include "../includes/audit_results_view.hpp"
#include "../includes/ide.hpp"
#include "../includes/line_number_area.hpp"
#include "../includes/syntax_highlighter.hpp"
#include <iostream>
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
#include <QSizePolicy>
#include <QDir>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QTextEdit>
#include <QInputDialog>
#include <QDialog>
#include <QLineEdit>
#include <QKeyEvent>

/**
 * @brief Constructs the MainWindow object.
 * @param parent The parent widget.
 */
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
    , outputDisplay(new OutputDisplay(this))
    , toggleCliPanelAction(new QAction("CTrace Options", this))
    , saveAction(new QAction("Save", this))
    , importAction(new QAction("Open Folder", this))
    , autosaveAction(new QAction("Autosave", this))
    , autosaveEnabled(false)
    , currentFilePath("")
    , syntaxHighlighter(nullptr)
{
    setupUi();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupCentralWidget();

    // Set initial file tree view to current working directory
    QString currentPath = QDir::currentPath();
    fileTree->setRootPath(currentPath);
    fileTree->setCurrentIndex(fileSystemModel->index(currentPath));
    statusBar()->showMessage("Current directory: " + currentPath);
}

/**
 * @brief Destructor for MainWindow.
 */
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

/**
 * @brief Sets up the UI for the main window.
 */
void MainWindow::setupUi()
{
    setWindowTitle("CoreTrace IDE");
    resize(800, 600);
}

/**
 * @brief Sets up the menu bar and its actions.
 */
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

/**
 * @brief Sets up the toolbar and its actions.
 */
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

/**
 * @brief Sets up the status bar.
 */
void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready");
}

/**
 * @brief Sets up the central widget and its layout.
 */
void MainWindow::setupCentralWidget()
{
    if (centralWidget) {
        delete centralWidget;
    }
    centralWidget = new QWidget(this);
    
    if (mainLayout) {
        delete mainLayout;
    }
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Create main splitter
    if (mainSplitter) {
        delete mainSplitter;
    }
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Create left container for file tree and output display
    QWidget* leftContainer = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(5);
    
    // Add file tree to left container
    fileTree->setMinimumWidth(150);
    fileTree->setMaximumWidth(400);
    fileTree->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    leftLayout->addWidget(fileTree);
    
    // Add output display to left container (initially hidden)
    outputDisplay->setMinimumWidth(150);
    outputDisplay->setMaximumWidth(400);
    outputDisplay->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    outputDisplay->setVisible(false);  // Hide initially
    leftLayout->addWidget(outputDisplay);
    
    // Add left container to splitter
    mainSplitter->addWidget(leftContainer);
    
    // Create a container for the text editor and line numbers
    QWidget* editorContainer = new QWidget(this);
    QHBoxLayout* editorLayout = new QHBoxLayout(editorContainer);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    editorLayout->setSpacing(0);
    
    // Create and add line number area
    LineNumberArea* lineNumberArea = new LineNumberArea(textEditor);
    editorLayout->addWidget(lineNumberArea);
    
    // Add text editor to the container
    textEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    editorLayout->addWidget(textEditor);

    // Create syntax highlighter
    syntaxHighlighter = new CppHighlighter(textEditor->document());
    
    // Add editor container to splitter
    mainSplitter->addWidget(editorContainer);
    
    // Add CLI panel to the right side of the splitter
    cliPanel->setMinimumWidth(150);
    cliPanel->setMaximumWidth(400);
    cliPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    cliPanel->setVisible(false);
    mainSplitter->addWidget(cliPanel);
    
    // Set the splitter as the main layout
    mainLayout->addWidget(mainSplitter);
    setCentralWidget(centralWidget);
    
    // Set initial splitter sizes and stretch factors
    QList<int> sizes;
    sizes << 200 << 600 << 0;  // Left panel, center, right panel
    mainSplitter->setSizes(sizes);
    mainSplitter->setStretchFactor(0, 0);  // Left panel doesn't stretch
    mainSplitter->setStretchFactor(1, 1);  // Center panel stretches
    mainSplitter->setStretchFactor(2, 0);  // Right panel doesn't stretch
    
    // Set splitter handle properties
    mainSplitter->setHandleWidth(5);
    mainSplitter->setChildrenCollapsible(false);
    
    // Connect text editor signals to update line numbers
    connect(textEditor->document(), &QTextDocument::blockCountChanged, this, [lineNumberArea]() {
        lineNumberArea->update();
    });
    
    connect(textEditor->verticalScrollBar(), &QScrollBar::valueChanged, this, [lineNumberArea]() {
        lineNumberArea->update();
    });
    
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
        if (currentFilePath.isEmpty()) {
            QMessageBox::warning(this, "No File Selected", 
                "Please select a file to analyze first.");
            return;
        }

        // Save current file if modified
        if (textEditor->document()->isModified()) {
            saveCurrentFile();
        }

        // Get IDE instance and perform audit with the selected options
        IDE* ide = IDE::getInstance();
        QList<AuditResult> results = ide->getAuditService()->performAudit(currentFilePath, options);
        
        // Debug logging
        std::cout << "Executing with options: " << options.toStdString() << std::endl;
        std::cout << "Number of results: " << results.size() << std::endl;
        if (!results.isEmpty()) {
            std::cout << "First result message: " << results[0].getMessage().toStdString() << std::endl;
        }
        
        // Show the output display if it was hidden
        if (!outputDisplay->isVisible()) {
            outputDisplay->setVisible(true);
            // Adjust splitter sizes to accommodate the output display
            QList<int> sizes = mainSplitter->sizes();
            sizes[0] = 300;  // Increase left panel width
            mainSplitter->setSizes(sizes);
        }
        
        // Display results in status bar
        QString resultMessage = QString("Analysis complete: %1 issues found").arg(results.size());
        statusBar()->showMessage(resultMessage);
        
        // Show results in output display
        QString outputText;
        if (results.isEmpty()) {
            outputText = "No issues found.";
        } else {
            outputText = results[0].getMessage();
        }
        outputDisplay->setOutput(outputText);
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

/**
 * @brief Event filter to handle keyboard shortcuts and focus events.
 * @param obj The object receiving the event.
 * @param event The event to be processed.
 * @return True if the event was handled, otherwise false.
 */
bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == textEditor) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            // Capture Ctrl+S
            if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_S) {
                this->saveCurrentFile();
                return true; // Event has been handled
            }
            // Capture Ctrl+F
            if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_F) {
                this->findNext();
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

/**
 * @brief Toggles the visibility of the CLI panel.
 */
void MainWindow::toggleCliPanel()
{
    bool isVisible = cliPanel->isVisible();
    cliPanel->setVisible(!isVisible);
    
    // Update splitter sizes
    QList<int> sizes = mainSplitter->sizes();
    if (!isVisible) {
        // Show panel: give it 200 pixels width
        sizes[2] = 200;
    } else {
        // Hide panel: set width to 0
        sizes[2] = 0;
    }
    mainSplitter->setSizes(sizes);
    
    // Update action text
    toggleCliPanelAction->setText(isVisible ? "Show CLI Panel" : "Hide CLI Panel");
}

/**
 * @brief Displays the main window.
 */
void MainWindow::show() {
    QMainWindow::show();
}

/**
 * @brief Updates the audit results in the UI.
 * @param results The list of audit results to display.
 */
void MainWindow::updateAuditResults(const QList<AuditResult>& results) {
    for (UIComponent* component : uiComponents) {
        if (AuditResultsView* resultsView = dynamic_cast<AuditResultsView*>(component)) {
            resultsView->displayResults(results);
        }
    }
}

/**
 * @brief Adds a tool button to the toolbar.
 * @param text The button text.
 * @param toolTip The tooltip text.
 * @param icon The button icon.
 * @param slot The slot to connect the button to.
 * @param receiver The object receiving the signal.
 */
void MainWindow::addToolButton(const QString& text, const QString& toolTip, 
                             const QIcon& icon, const char* slot, QObject* receiver) {
    QAction* action = new QAction(icon, text, this);
    action->setToolTip(toolTip);
    connect(action, SIGNAL(triggered()), receiver, slot);
    mainToolBar->addAction(action);
}

/**
 * @brief Adds a separator to the toolbar.
 */
void MainWindow::addToolSeparator() {
    mainToolBar->addSeparator();
}

/**
 * @brief Adds a widget to the main layout.
 * @param widget The widget to add.
 * @param alignment The alignment of the widget.
 */
void MainWindow::addWidget(QWidget* widget, Qt::Alignment alignment) {
    mainLayout->addWidget(widget, 0, alignment);
}

/**
 * @brief Sets the root path for the project.
 * @param path The root path to set.
 */
void MainWindow::setProjectRoot(const QString& path) {
    fileTree->setRootPath(path);
}

/**
 * @brief Saves the current file being edited.
 */
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

/**
 * @brief Opens a folder and sets it as the project root.
 */
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

/**
 * @brief Toggles the autosave feature.
 */
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

/**
 * @brief Opens a search dialog and highlights all occurrences of the search term.
 */
void MainWindow::findNext() {
    // Create a custom dialog
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Find Text");
    dialog->setFixedSize(300, 50);
    
    // Create layout
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(10, 10, 10, 10);
    
    // Create line edit
    QLineEdit* searchEdit = new QLineEdit(dialog);
    searchEdit->setPlaceholderText("Enter text to find...");
    layout->addWidget(searchEdit);
    
    // Set focus to line edit
    searchEdit->setFocus();
    
    // Handle Enter key
    connect(searchEdit, &QLineEdit::returnPressed, dialog, &QDialog::accept);
    
    // Handle Escape key
    QShortcut* escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), dialog);
    connect(escapeShortcut, &QShortcut::activated, dialog, &QDialog::reject);
    
    // Show dialog
    if (dialog->exec() == QDialog::Accepted && !searchEdit->text().isEmpty()) {
        QString searchTerm = searchEdit->text();
        
        // Clear previous highlights
        QList<QTextEdit::ExtraSelection> extraSelections;
        QTextEdit::ExtraSelection selection;
        
        // Create a more visible highlight color
        QColor highlightColor(255, 255, 0, 100);  // Semi-transparent yellow
        selection.format.setBackground(highlightColor);
        selection.format.setForeground(Qt::black);  // Ensure text remains readable
        
        // Find all occurrences
        QTextCursor cursor = textEditor->document()->find(searchTerm);
        while (!cursor.isNull()) {
            selection.cursor = cursor;
            extraSelections.append(selection);
            cursor = textEditor->document()->find(searchTerm, cursor);
        }
        
        // Apply highlights
        textEditor->setExtraSelections(extraSelections);
        
        // Move to first occurrence
        if (!extraSelections.isEmpty()) {
            textEditor->setTextCursor(extraSelections.first().cursor);
        }
        
        statusBar()->showMessage(QString("Found %1 occurrences").arg(extraSelections.size()), 3000);
    }
    
    delete dialog;
}