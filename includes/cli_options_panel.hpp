#ifndef CLI_OPTIONS_PANEL_HPP
#define CLI_OPTIONS_PANEL_HPP

#include <QWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>

class CliOptionsPanel : public QWidget {
    Q_OBJECT

public:
    explicit CliOptionsPanel(QWidget* parent = nullptr);
    ~CliOptionsPanel();

    QString getCommandOptions() const;

signals:
    void executeClicked(const QString& options);

private:
    QCheckBox* symbolicCheck;
    QCheckBox* staticCheck;
    QCheckBox* dynamicCheck;
    QCheckBox* allCheck;
    QSpinBox* verboseSpin;
    QPushButton* executeButton;

    void setupUi();
    void connectSignals();
};

#endif // CLI_OPTIONS_PANEL_HPP 