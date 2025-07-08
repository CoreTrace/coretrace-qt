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
#include <QFileInfo>
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
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QEasingCurve>

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
    , importAction(new QAction("Import", this))
    , autosaveAction(new QAction("Autosave", this))
    , autosaveEnabled(false)
    , syntaxHighlighter(nullptr)
    , securityNotificationFrame(nullptr)
    , securityNotificationIcon(nullptr)
    , securityNotificationText(nullptr)
    , notificationTimer(new QTimer(this))
    , notificationAnimation(nullptr)
    , notificationOpacity(nullptr)
    , blinkTimer(new QTimer(this))
    , blinkState(false)
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
    
    // Initialize the security notification system
    createSecurityNotificationWidget();
    
    // Connect text editor resize event to update notification position
    connect(textEditor, &QTextEdit::textChanged, [this]() {
        if (securityNotificationFrame && securityNotificationFrame->isVisible()) {
            securityNotificationFrame->resize(textEditor->width() - 40, 
                                             securityNotificationFrame->height());
            securityNotificationFrame->move(20, 20);
        }
    });
    
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
    // Clear previous security highlights
    clearSecurityHighlights();
    
    // Update the results view
    for (UIComponent* component : uiComponents) {
        if (AuditResultsView* resultsView = dynamic_cast<AuditResultsView*>(component)) {
            resultsView->displayResults(results);
        }
    }
    
    // Highlight security issues in the text editor
    bool foundSecurityIssues = false;
    for (const AuditResult& result : results) {
        if (result.hasLocationInfo() && !result.getRuleId().isEmpty()) {
            // Check if this result is for the currently open file
            QString currentFile = QFileInfo(currentFilePath).fileName();
            QString resultFile = QFileInfo(result.getFilePath()).fileName();
            
            if (currentFile == resultFile || result.getFilePath().contains(currentFile)) {
                highlightSecurityIssue(result);
                foundSecurityIssues = true;
            }
        }
    }
    
    // Show summary message
    if (foundSecurityIssues) {
        statusBar()->showMessage(QString("Found security issues - highlighted in editor"), 8000);
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

/**
 * @brief Highlights a security issue in the text editor with enhanced visual effects.
 * @param result The audit result containing location information.
 */
void MainWindow::highlightSecurityIssue(const AuditResult& result) {
    if (!result.hasLocationInfo()) {
        return; // No location information available
    }
    
    QTextDocument* document = textEditor->document();
    QTextCursor cursor(document);
    
    // Move to the specified line (convert from 1-based to 0-based)
    cursor.movePosition(QTextCursor::Start);
    for (int i = 1; i < result.getLine(); ++i) {
        if (!cursor.movePosition(QTextCursor::Down)) {
            return; // Line not found
        }
    }
    
    // Get the severity level for different visual treatments
    QString ruleId = result.getRuleId();
    bool isError = ruleId.contains("error", Qt::CaseInsensitive) || 
                   result.getMessage().contains("error", Qt::CaseInsensitive);
    bool isWarning = ruleId.contains("warning", Qt::CaseInsensitive) || 
                     result.getMessage().contains("warning", Qt::CaseInsensitive) ||
                     result.getMessage().contains("CWE", Qt::CaseInsensitive);
    
    QString severity = isError ? "error" : (isWarning ? "warning" : "info");
    
    // Create multiple highlighting effects for maximum visibility
    QList<QTextEdit::ExtraSelection> newHighlights;
    
    // 1. Highlight the entire line with a subtle background
    QTextCursor lineHighlightCursor = cursor;
    lineHighlightCursor.movePosition(QTextCursor::StartOfLine);
    lineHighlightCursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    
    QTextEdit::ExtraSelection lineHighlight;
    lineHighlight.cursor = lineHighlightCursor;
    if (isError) {
        lineHighlight.format.setBackground(QColor(255, 235, 235, 140)); // Slightly more visible for errors
        lineHighlight.format.setProperty(QTextFormat::FullWidthSelection, true);
    } else if (isWarning) {
        lineHighlight.format.setBackground(QColor(255, 248, 220, 140)); // Slightly more visible for warnings
        lineHighlight.format.setProperty(QTextFormat::FullWidthSelection, true);
    } else {
        lineHighlight.format.setBackground(QColor(245, 245, 255, 140)); // Slightly more visible for info
        lineHighlight.format.setProperty(QTextFormat::FullWidthSelection, true);
    }
    newHighlights.append(lineHighlight);
    
    // 2. Highlight the specific problematic code section
    QTextCursor specificCursor = cursor;
    
    // Move to the specified column if available
    if (result.getColumn() > 0) {
        specificCursor.movePosition(QTextCursor::StartOfLine);
        specificCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, result.getColumn() - 1);
    }
    
    // Select the problematic text with enhanced logic
    int selectionLength = 1;
    if (result.getEndColumn() > result.getColumn()) {
        selectionLength = result.getEndColumn() - result.getColumn();
    } else {
        // Enhanced text selection logic
        QString lineText = specificCursor.block().text();
        int startPos = specificCursor.positionInBlock();
        
        // Try to select meaningful code constructs
        if (startPos < lineText.length()) {
            QChar currentChar = lineText[startPos];
            
            // If we're at a function call, select the whole function
            if (currentChar.isLetter() || currentChar == '_') {
                // Select identifier
                while (startPos + selectionLength < lineText.length() && 
                       (lineText[startPos + selectionLength].isLetterOrNumber() || 
                        lineText[startPos + selectionLength] == '_')) {
                    selectionLength++;
                }
                
                // If followed by '(', include the parentheses and arguments
                if (startPos + selectionLength < lineText.length() && 
                    lineText[startPos + selectionLength] == '(') {
                    int parenCount = 0;
                    while (startPos + selectionLength < lineText.length()) {
                        QChar c = lineText[startPos + selectionLength];
                        selectionLength++;
                        if (c == '(') parenCount++;
                        else if (c == ')') {
                            parenCount--;
                            if (parenCount == 0) break;
                        }
                    }
                }
            }
            // If we're at a string literal, select the whole string
            else if (currentChar == '"' || currentChar == '\'') {
                QChar quote = currentChar;
                selectionLength = 1;
                while (startPos + selectionLength < lineText.length()) {
                    QChar c = lineText[startPos + selectionLength];
                    selectionLength++;
                    if (c == quote && lineText[startPos + selectionLength - 2] != '\\') {
                        break;
                    }
                }
            }
        }
        
        // Ensure minimum visibility
        if (selectionLength < 3) {
            selectionLength = qMin(10, lineText.length() - startPos);
        }
    }
    
    specificCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, selectionLength);
    
    // Create enhanced specific highlight with more prominent styling
    QTextEdit::ExtraSelection specificHighlight;
    specificHighlight.cursor = specificCursor;
    
    if (isError) {
        // Error styling - bright red with strong emphasis and border
        specificHighlight.format.setBackground(QColor(255, 80, 80, 220));
        specificHighlight.format.setForeground(QColor(255, 255, 255));
        specificHighlight.format.setFontWeight(QFont::Bold);
        specificHighlight.format.setUnderlineColor(QColor(200, 0, 0));
        specificHighlight.format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        specificHighlight.format.setProperty(QTextFormat::OutlinePen, QPen(QColor(150, 0, 0), 3));
    } else if (isWarning) {
        // Warning styling - orange/yellow with medium emphasis and border
        specificHighlight.format.setBackground(QColor(255, 140, 0, 200));
        specificHighlight.format.setForeground(QColor(255, 255, 255));
        specificHighlight.format.setFontWeight(QFont::Bold);
        specificHighlight.format.setUnderlineColor(QColor(255, 140, 0));
        specificHighlight.format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        specificHighlight.format.setProperty(QTextFormat::OutlinePen, QPen(QColor(200, 100, 0), 2));
    } else {
        // Info styling - blue with subtle emphasis and border
        specificHighlight.format.setBackground(QColor(100, 150, 255, 170));
        specificHighlight.format.setForeground(QColor(255, 255, 255));
        specificHighlight.format.setFontWeight(QFont::Bold);
        specificHighlight.format.setUnderlineColor(QColor(0, 100, 200));
        specificHighlight.format.setUnderlineStyle(QTextCharFormat::DotLine);
        specificHighlight.format.setProperty(QTextFormat::OutlinePen, QPen(QColor(0, 100, 200), 1));
    }
    
    // Enhanced tooltip with more information and emojis
    QString tooltip = QString("🚨 SECURITY ISSUE DETECTED 🚨\n\n"
                             "🔍 Rule: %1\n"
                             "📝 Description: %2\n"
                             "📍 Location: Line %3, Column %4\n\n"
                             "💡 Click to see full details in results panel")
                     .arg(result.getRuleId())
                     .arg(result.getMessage().split('\n').first()) // First line of message
                     .arg(result.getLine())
                     .arg(result.getColumn());
    specificHighlight.format.setToolTip(tooltip);
    
    newHighlights.append(specificHighlight);
    
    // 3. Add a prominent left margin indicator with icon-like effect
    QTextCursor marginCursor = cursor;
    marginCursor.movePosition(QTextCursor::StartOfLine);
    marginCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2);
    
    QTextEdit::ExtraSelection marginHighlight;
    marginHighlight.cursor = marginCursor;
    if (isError) {
        marginHighlight.format.setBackground(QColor(255, 0, 0, 150));
        marginHighlight.format.setProperty(QTextFormat::OutlinePen, QPen(QColor(200, 0, 0), 4));
    } else if (isWarning) {
        marginHighlight.format.setBackground(QColor(255, 165, 0, 150));
        marginHighlight.format.setProperty(QTextFormat::OutlinePen, QPen(QColor(200, 100, 0), 3));
    } else {
        marginHighlight.format.setBackground(QColor(0, 100, 255, 150));
        marginHighlight.format.setProperty(QTextFormat::OutlinePen, QPen(QColor(0, 100, 200), 2));
    }
    newHighlights.append(marginHighlight);
    
    // Store current highlights
    currentHighlights.append(newHighlights);
    
    // Get existing selections and add the new highlights
    QList<QTextEdit::ExtraSelection> extraSelections = textEditor->extraSelections();
    extraSelections.append(newHighlights);
    textEditor->setExtraSelections(extraSelections);
    
    // Move cursor to the highlighted issue for visibility
    textEditor->setTextCursor(specificCursor);
    textEditor->ensureCursorVisible();
    
    // Show prominent security notification
    showSecurityNotification(result);
    
    // Start animated blinking effect
    animateHighlight(specificCursor, severity);
    
    // Enhanced status message with emojis
    QString severityIcon = isError ? "🔴" : (isWarning ? "🟡" : "🔵");
    statusBar()->showMessage(QString("%1 SECURITY ISSUE: %2 at line %3 - CHECK NOTIFICATION ABOVE!")
                           .arg(severityIcon)
                           .arg(result.getRuleId())
                           .arg(result.getLine()), 15000);
}

/**
 * @brief Clears all security issue highlights from the text editor.
 */
void MainWindow::clearSecurityHighlights() {
    // Clear current highlights list
    currentHighlights.clear();
    
    // Clear all extra selections (this will remove both search and security highlights)
    textEditor->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    
    // Stop any blinking animation
    if (blinkTimer->isActive()) {
        blinkTimer->stop();
    }
    
    // Hide notification
    hideSecurityNotification();
    
    statusBar()->showMessage("Security highlights cleared", 2000);
}

/**
 * @brief Creates and sets up the security notification widget.
 */
void MainWindow::createSecurityNotificationWidget() {
    if (securityNotificationFrame) {
        return; // Already created
    }
    
    // Create notification frame
    securityNotificationFrame = new QFrame(this);
    securityNotificationFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    securityNotificationFrame->setLineWidth(2);
    securityNotificationFrame->setStyleSheet(
        "QFrame {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "                 stop:0 #ffebee, stop:1 #ffcdd2);"
        "    border: 2px solid #f44336;"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "}"
    );
    securityNotificationFrame->hide();
    
    // Create layout for notification
    QHBoxLayout* notificationLayout = new QHBoxLayout(securityNotificationFrame);
    notificationLayout->setContentsMargins(12, 8, 12, 8);
    notificationLayout->setSpacing(12);
    
    // Create icon label
    securityNotificationIcon = new QLabel(securityNotificationFrame);
    securityNotificationIcon->setFixedSize(24, 24);
    securityNotificationIcon->setScaledContents(true);
    notificationLayout->addWidget(securityNotificationIcon);
    
    // Create text label
    securityNotificationText = new QLabel(securityNotificationFrame);
    securityNotificationText->setWordWrap(true);
    securityNotificationText->setStyleSheet(
        "QLabel {"
        "    color: #d32f2f;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "    background: transparent;"
        "    border: none;"
        "}"
    );
    notificationLayout->addWidget(securityNotificationText, 1);
    
    // Create close button
    QPushButton* closeButton = new QPushButton("×", securityNotificationFrame);
    closeButton->setFixedSize(20, 20);
    closeButton->setStyleSheet(
        "QPushButton {"
        "    background: #f44336;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 10px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background: #d32f2f;"
        "}"
    );
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::hideSecurityNotification);
    notificationLayout->addWidget(closeButton);
    
    // Position the notification at the top of the text editor
    securityNotificationFrame->setParent(textEditor);
    securityNotificationFrame->resize(textEditor->width() - 40, 60);
    securityNotificationFrame->move(20, 20);
    
    // Set up opacity effect
    notificationOpacity = new QGraphicsOpacityEffect(this);
    securityNotificationFrame->setGraphicsEffect(notificationOpacity);
    
    // Set up animation
    notificationAnimation = new QPropertyAnimation(notificationOpacity, "opacity", this);
    notificationAnimation->setDuration(300);
    notificationAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    
    // Set up auto-hide timer
    notificationTimer->setSingleShot(true);
    notificationTimer->setInterval(10000); // Hide after 10 seconds
    connect(notificationTimer, &QTimer::timeout, this, &MainWindow::hideSecurityNotification);
}

/**
 * @brief Shows a prominent security notification for the given result.
 * @param result The audit result to display.
 */
void MainWindow::showSecurityNotification(const AuditResult& result) {
    createSecurityNotificationWidget();
    
    // Determine severity and set appropriate icon
    QString ruleId = result.getRuleId();
    bool isError = ruleId.contains("error", Qt::CaseInsensitive) || 
                   result.getMessage().contains("error", Qt::CaseInsensitive);
    bool isWarning = ruleId.contains("warning", Qt::CaseInsensitive) || 
                     result.getMessage().contains("warning", Qt::CaseInsensitive) ||
                     result.getMessage().contains("CWE", Qt::CaseInsensitive);
    
    // Set icon based on severity
    QPixmap iconPixmap;
    QString styleSheet;
    if (isError) {
        iconPixmap = QPixmap(":/icons/error.png");
        styleSheet = "QFrame {"
                    "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                    "                 stop:0 #ffebee, stop:1 #ffcdd2);"
                    "    border: 2px solid #f44336;"
                    "    border-radius: 8px;"
                    "    padding: 8px;"
                    "}";
    } else if (isWarning) {
        iconPixmap = QPixmap(":/icons/warning.png");
        styleSheet = "QFrame {"
                    "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                    "                 stop:0 #fff8e1, stop:1 #ffecb3);"
                    "    border: 2px solid #ff9800;"
                    "    border-radius: 8px;"
                    "    padding: 8px;"
                    "}";
    } else {
        iconPixmap = QPixmap(":/icons/info.png");
        styleSheet = "QFrame {"
                    "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                    "                 stop:0 #e3f2fd, stop:1 #bbdefb);"
                    "    border: 2px solid #2196f3;"
                    "    border-radius: 8px;"
                    "    padding: 8px;"
                    "}";
    }
    
    securityNotificationFrame->setStyleSheet(styleSheet);
    securityNotificationIcon->setPixmap(iconPixmap);
    
    // Set notification text
    QString notificationText = QString("🚨 <b>%1</b><br>%2<br>📍 Line %3, Column %4")
                              .arg(result.getRuleId())
                              .arg(result.getMessage().left(100) + (result.getMessage().length() > 100 ? "..." : ""))
                              .arg(result.getLine())
                              .arg(result.getColumn());
    securityNotificationText->setText(notificationText);
    
    // Update notification position and size
    securityNotificationFrame->resize(textEditor->width() - 40, 
                                     securityNotificationText->heightForWidth(textEditor->width() - 120) + 30);
    securityNotificationFrame->move(20, 20);
    
    // Show with fade-in animation
    notificationOpacity->setOpacity(0);
    securityNotificationFrame->show();
    notificationAnimation->setStartValue(0.0);
    notificationAnimation->setEndValue(1.0);
    notificationAnimation->start();
    
    // Start auto-hide timer
    notificationTimer->start();
}

/**
 * @brief Hides the security notification with fade-out animation.
 */
void MainWindow::hideSecurityNotification() {
    if (!securityNotificationFrame || !securityNotificationFrame->isVisible()) {
        return;
    }
    
    notificationTimer->stop();
    
    // Fade out animation
    notificationAnimation->setStartValue(1.0);
    notificationAnimation->setEndValue(0.0);
    
    // Hide the frame when animation completes
    connect(notificationAnimation, &QPropertyAnimation::finished, 
            securityNotificationFrame, &QFrame::hide, Qt::UniqueConnection);
    
    notificationAnimation->start();
}

/**
 * @brief Creates an animated highlight effect for the specified cursor position.
 * @param cursor The text cursor indicating the position to animate.
 * @param severity The severity level for color selection.
 */
void MainWindow::animateHighlight(const QTextCursor& cursor, const QString& severity) {
    // Stop any existing blinking
    if (blinkTimer->isActive()) {
        blinkTimer->stop();
    }
    
    // Store the cursor for blinking animation
    QTextCursor animatedCursor = cursor;
    
    // Set up blinking timer
    blinkState = false;
    blinkTimer->setInterval(500); // Blink every 500ms
    
    // Create blinking effect
    connect(blinkTimer, &QTimer::timeout, [this, animatedCursor, severity]() {
        blinkState = !blinkState;
        
        if (blinkState) {
            // Show enhanced highlight
            QTextEdit::ExtraSelection blinkHighlight;
            blinkHighlight.cursor = animatedCursor;
            
            if (severity == "error") {
                blinkHighlight.format.setBackground(QColor(255, 0, 0, 200));
                blinkHighlight.format.setForeground(QColor(255, 255, 255));
            } else if (severity == "warning") {
                blinkHighlight.format.setBackground(QColor(255, 165, 0, 200));
                blinkHighlight.format.setForeground(QColor(255, 255, 255));
            } else {
                blinkHighlight.format.setBackground(QColor(0, 100, 255, 200));
                blinkHighlight.format.setForeground(QColor(255, 255, 255));
            }
            
            blinkHighlight.format.setFontWeight(QFont::Bold);
            blinkHighlight.format.setProperty(QTextFormat::OutlinePen, QPen(QColor(0, 0, 0), 3));
            
            QList<QTextEdit::ExtraSelection> tempSelections = currentHighlights;
            tempSelections.append(blinkHighlight);
            textEditor->setExtraSelections(tempSelections);
        } else {
            // Hide enhanced highlight, show normal highlights
            textEditor->setExtraSelections(currentHighlights);
        }
    });
    
    // Start blinking for 3 seconds
    blinkTimer->start();
    QTimer::singleShot(3000, [this]() {
        if (blinkTimer->isActive()) {
            blinkTimer->stop();
            textEditor->setExtraSelections(currentHighlights);
        }
    });
}