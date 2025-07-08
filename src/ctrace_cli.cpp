#include "../includes/ctrace_cli.hpp"
#include <QProcess>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <iostream>

/**
 * @brief Executes the CTrace CLI command with the specified file and options.
 * @param file The file to analyze.
 * @param options The command-line options for the analysis.
 * @return The standard output of the CTrace command.
 */
QString CTraceCLI::execute(const QString& file, const QString& options) {
    std::cout << "Executing CTrace CLI with options: " << options.toStdString() << std::endl;
    QProcess process;
    
    // Clear any existing cache/report files to force fresh analysis
    QString reportFile = "ctrace-report.txt";
    if (QFile::exists(reportFile)) {
        QFile::remove(reportFile);
        std::cout << "Removed existing report file: " << reportFile.toStdString() << std::endl;
    }
    
    // Split options into arguments
    QStringList arguments;
    arguments.append("--input");
    arguments.append(file);
    arguments.append("--sarif-format");
    
    if (!options.isEmpty()) {
        arguments.append(options.split(' ', Qt::SkipEmptyParts));
    }
    
    std::cout << "Full command: ctrace " << arguments.join(" ").toStdString() << std::endl;
    
    // Set working directory to the binary's location
    process.setWorkingDirectory(QDir::currentPath());
    
    // Use the correct binary name
    process.start("./coretrace/build/ctrace", arguments);
    
    // Wait for the process to finish with a timeout
    if (!process.waitForFinished(30000)) { // 30 second timeout
        return "Error: Process timed out";
    }
    
    // Check for errors
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        QString error = process.readAllStandardError();
        if (error.isEmpty()) {
            error = "Unknown error occurred";
        }
        std::cout << "Process failed with exit code: " << process.exitCode() << std::endl;
        std::cout << "Error output: " << error.toStdString() << std::endl;
        return "Error: " + error;
    }
    
    // Get both stdout and stderr
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();
    
    if (!error.isEmpty()) {
        output += "\nErrors:\n" + error;
    }
    
    return output;
}