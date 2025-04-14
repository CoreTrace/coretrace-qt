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
    delete mainSplitter;
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

    QAction* exitAction = menuBar->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
}

void MainWindow::setupToolBar()
{
    mainToolBar = addToolBar("Main Toolbar");
    mainToolBar->setMovable(false);
    mainToolBar->setFloatable(false);

    // Add toggle button for CLI panel
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
    textEditor->setReadOnly(true);
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
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            textEditor->setPlainText(in.readAll());
            file.close();
        }
    });

    // Connect CLI panel execute signal
    connect(cliPanel, &CliOptionsPanel::executeClicked, this, [this](const QString& options) {
        statusBar()->showMessage("Executing CTrace with options: " + options);
    });
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