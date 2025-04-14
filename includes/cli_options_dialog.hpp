#ifndef CLI_OPTIONS_DIALOG_HPP
#define CLI_OPTIONS_DIALOG_HPP

#include <QDialog>
#include <QCheckBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>

class CliOptionsDialog : public QDialog {
    Q_OBJECT

public:
    explicit CliOptionsDialog(QWidget* parent = nullptr);
    ~CliOptionsDialog();

    QString getCommandOptions() const;

private:
    QCheckBox* symbolicCheck;
    QCheckBox* staticCheck;
    QCheckBox* dynamicCheck;
    QCheckBox* allCheck;
    QSpinBox* verboseSpin;
    QDialogButtonBox* buttonBox;

    void setupUi();
    void connectSignals();
};

#endif // CLI_OPTIONS_DIALOG_HPP 