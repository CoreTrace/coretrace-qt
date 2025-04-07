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
    , fileTree(new FileTreeView(this))
    , textEditor(new QTextEdit(this))
    , fileSystemModel(new QFileSystemModel(this))
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

    // File menu
    QMenu* fileMenu = menuBar->addMenu("Options");
    QAction* exitAction = fileMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
}

void MainWindow::setupToolBar()
{
    mainToolBar = addToolBar("Main Toolbar");
    mainToolBar->setMovable(false);
    mainToolBar->setFloatable(false);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready");
}

void MainWindow::setupCentralWidget()
{
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(centralWidget);
    
    // Create a splitter to separate the file tree and text editor
    QSplitter* splitter = new QSplitter(Qt::Horizontal, centralWidget);
    
    // Add file tree to the splitter with fixed width
    fileTree->setFixedWidth(250);  // Set fixed width to 250 pixels
    splitter->addWidget(fileTree);
    
    // Add text editor to the splitter
    textEditor->setReadOnly(true);
    splitter->addWidget(textEditor);
    
    // Set the splitter as the main layout
    layout->addWidget(splitter);
    setCentralWidget(centralWidget);
    
    // Set initial splitter sizes (file tree takes 250 pixels, rest goes to text editor)
    QList<int> sizes;
    sizes << 250 << width() - 250;
    splitter->setSizes(sizes);
    
    // Connect file selection signal
    connect(fileTree, &FileTreeView::fileSelected, this, [this](const QString& filePath) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            textEditor->setPlainText(in.readAll());
            file.close();
        }
    });
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