#include "../includes/cli_options_dialog.hpp"
#include <QGroupBox>
#include <QFormLayout>

/**
 * @brief Constructs the CLI options dialog.
 * @param parent The parent widget.
 */
CliOptionsDialog::CliOptionsDialog(QWidget* parent)
    : QDialog(parent)
    , symbolicCheck(new QCheckBox("Symbolic Analysis", this))
    , staticCheck(new QCheckBox("Static Analysis", this))
    , dynamicCheck(new QCheckBox("Dynamic Analysis", this))
    , allCheck(new QCheckBox("All Analyses", this))
    , verboseSpin(new QSpinBox(this))
    , buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this))
{
    setupUi();
    connectSignals();
}

/**
 * @brief Destructor for the CLI options dialog.
 */
CliOptionsDialog::~CliOptionsDialog()
{
    delete symbolicCheck;
    delete staticCheck;
    delete dynamicCheck;
    delete allCheck;
    delete verboseSpin;
    delete buttonBox;
}

/**
 * @brief Sets up the UI for the CLI options dialog.
 */
void CliOptionsDialog::setupUi()
{
    setWindowTitle("CTrace Analysis Options");
    resize(400, 300);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

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
    verboseSpin->setValue(1);
    verboseLayout->addRow("Level:", verboseSpin);

    mainLayout->addWidget(analysisGroup);
    mainLayout->addWidget(verboseGroup);
    mainLayout->addWidget(buttonBox);
}

/**
 * @brief Connects signals and slots for the CLI options dialog.
 */
void CliOptionsDialog::connectSignals()
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

    // Connect dialog buttons
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

/**
 * @brief Retrieves the command-line options based on the selected settings.
 * @return A string containing the command-line options.
 */
QString CliOptionsDialog::getCommandOptions() const
{
    QStringList options;

    if (allCheck->isChecked()) {
        options << "--all";
    } else {
        if (symbolicCheck->isChecked()) options << "--symbolic";
        if (staticCheck->isChecked()) options << "--static";
        if (dynamicCheck->isChecked()) options << "--dynamic";
    }

    if (verboseSpin->value() > 0) {
        options << "--verbose" << QString::number(verboseSpin->value());
    }

    return options.join(" ");
}