#include "../includes/cli_options_panel.hpp"
#include <QFormLayout>
#include <QStyle>
#include <QLabel>
#include <QPixmap>
#include <QPalette>

/**
 * @brief Constructs the CLI options panel.
 * @param parent The parent widget.
 */
CliOptionsPanel::CliOptionsPanel(QWidget* parent)
    : QWidget(parent)
    , symbolicCheck(new QCheckBox("Symbolic Analysis", this))
    , staticCheck(new QCheckBox("Static Analysis", this))
    , dynamicCheck(new QCheckBox("Dynamic Analysis", this))
    , allCheck(new QCheckBox("All Analyses", this))
    , verboseSpin(new QSpinBox(this))
    , executeButton(new QPushButton("Execute", this))
{
    setupUi();
    connectSignals();
}

/**
 * @brief Destructor for the CLI options panel.
 */
CliOptionsPanel::~CliOptionsPanel()
{
    delete symbolicCheck;
    delete staticCheck;
    delete dynamicCheck;
    delete allCheck;
    delete verboseSpin;
    delete executeButton;
}

/**
 * @brief Sets up the UI for the CLI options panel.
 */
void CliOptionsPanel::setupUi()
{
    setFixedWidth(250);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Add CoreTrace logo at the top
    QLabel* logoLabel = new QLabel(this);
    QPixmap logo(":/resources/coretrace_logo.png");
    if (!logo.isNull()) {
        // Scale the logo to fit the panel width
        QPixmap scaledLogo = logo.scaled(220, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        logoLabel->setPixmap(scaledLogo);
        logoLabel->setAlignment(Qt::AlignCenter);
        logoLabel->setStyleSheet(
            "QLabel {"
            "    background-color: transparent;"
            "    margin: 5px 0px 15px 0px;"
            "    padding: 5px;"
            "}"
        );
        mainLayout->addWidget(logoLabel);
    } else {
        // Fallback to styled text if logo doesn't load
        QLabel* titleLabel = new QLabel("CoreTrace Options", this);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet(
            "QLabel {"
            "    font-size: 16px;"
            "    font-weight: bold;"
            "    color: #0078d4;"
            "    margin: 5px 0px 15px 0px;"
            "    padding: 8px;"
            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "               stop:0 #e3f2fd, stop:1 #bbdefb);"
            "    border: 1px solid #2196f3;"
            "    border-radius: 6px;"
            "}"
        );
        mainLayout->addWidget(titleLabel);
    }

    // Analysis options group
    QGroupBox* analysisGroup = new QGroupBox("Analysis Types", this);
    QVBoxLayout* analysisLayout = new QVBoxLayout(analysisGroup);
    
    analysisLayout->addWidget(symbolicCheck);
    analysisLayout->addWidget(staticCheck);
    analysisLayout->addWidget(dynamicCheck);
    analysisLayout->addWidget(allCheck);
    
    // Verbose level
    QGroupBox* verboseGroup = new QGroupBox("Verbose Level", this);
    QFormLayout* verboseLayout = new QFormLayout(verboseGroup);
    verboseSpin->setRange(0, 3);
    verboseSpin->setValue(0);
    verboseLayout->addRow("Level:", verboseSpin);

    // Execute button
    executeButton->setStyleSheet("QPushButton {"
                                "background-color: #4CAF50;"
                                "color: white;"
                                "border: none;"
                                "padding: 8px;"
                                "border-radius: 4px;"
                                "}"
                                "QPushButton:hover {"
                                "background-color: #45a049;"
                                "}"
                                "QPushButton:pressed {"
                                "background-color: #3d8b40;"
                                "}");

    mainLayout->addWidget(analysisGroup);
    mainLayout->addWidget(verboseGroup);
    mainLayout->addStretch();
    mainLayout->addWidget(executeButton);
}

/**
 * @brief Connects signals and slots for the CLI options panel.
 */
void CliOptionsPanel::connectSignals()
{
    // Connect all checkbox to disable others when checked
    connect(allCheck, &QCheckBox::stateChanged, this, [this](int state) {
        bool enabled = state != Qt::Checked;
        symbolicCheck->setEnabled(enabled);
        staticCheck->setEnabled(enabled);
        dynamicCheck->setEnabled(enabled);
    });

    // Connect individual checkboxes to disable "all" when any is checked
    auto disableAll = [this]() {
        if (symbolicCheck->isChecked() || staticCheck->isChecked() || dynamicCheck->isChecked()) {
            allCheck->setChecked(false);
        }
    };
    
    connect(symbolicCheck, &QCheckBox::stateChanged, this, disableAll);
    connect(staticCheck, &QCheckBox::stateChanged, this, disableAll);
    connect(dynamicCheck, &QCheckBox::stateChanged, this, disableAll);

    // Connect execute button
    connect(executeButton, &QPushButton::clicked, this, [this]() {
        emit executeClicked(getCommandOptions());
    });
}

/**
 * @brief Retrieves the command-line options based on the selected settings.
 * @return A string containing the command-line options.
 */
QString CliOptionsPanel::getCommandOptions() const
{
    QStringList options;

    // Only add individual options, never use --all
    if (symbolicCheck->isChecked()) options << "--symbolic";
    if (staticCheck->isChecked()) options << "--static";
    if (dynamicCheck->isChecked()) options << "--dynamic";

    if (verboseSpin->value() > 0) {
        options << "--verbose" << QString::number(verboseSpin->value());
    }

    return options.join(" ");
}