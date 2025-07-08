#include "../includes/cli_options_panel.hpp"
#include <QFormLayout>
#include <QStyle>

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