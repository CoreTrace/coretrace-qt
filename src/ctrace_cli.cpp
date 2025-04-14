#include "../includes/ctrace_cli.hpp"
#include <QProcess>

QString CTraceCLI::execute(const QString& file, const QString& options) {
    QProcess process;
    QString command = "ctrace " + options + " " + file;
    process.start(command);
    process.waitForFinished();
    return QString(process.readAllStandardOutput());
} 