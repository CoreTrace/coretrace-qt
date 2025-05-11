#include "../includes/ctrace_cli.hpp"
#include <QProcess>

/**
 * @brief Executes the CTrace CLI command with the specified file and options.
 * @param file The file to analyze.
 * @param options The command-line options for the analysis.
 * @return The standard output of the CTrace command.
 */
QString CTraceCLI::execute(const QString& file, const QString& options) {
    QProcess process;
    QString command = "ctrace " + options + " " + file;
    process.start(command);
    process.waitForFinished();
    return QString(process.readAllStandardOutput());
}