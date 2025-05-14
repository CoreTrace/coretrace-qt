#include "../includes/ctrace_cli.hpp"
#include <QProcess>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>
#include <iostream>

// Checksum SHA-256 du binaire coretrace-cli validé
const QString CTraceCLI::EXPECTED_CHECKSUM = "777c27a9660e586ec52005b503eda59a507ab25d50f7261c3f8c12892ccb30ca"; 
const QString CTraceCLI::CLI_BINARY_PATH = "coretrace-cli";

CTraceCLI::CTraceCLI() {
}

bool CTraceCLI::verifyChecksum() {
    QString calculatedChecksum = calculateChecksum(CLI_BINARY_PATH);
    
    if (calculatedChecksum.isEmpty()) {
        QMessageBox::critical(nullptr, "Erreur critique", 
            "Impossible de trouver ou d'accéder au binaire coretrace-cli.\n"
            "Vérifiez que l'outil est correctement installé.");
        return false;
    }
    
    if (calculatedChecksum != EXPECTED_CHECKSUM) {
        QMessageBox::critical(nullptr, "Erreur de sécurité", 
            "Le binaire coretrace-cli a été modifié ou corrompu.\n"
            "Checksum attendu: " + EXPECTED_CHECKSUM + "\n"
            "Checksum calculé: " + calculatedChecksum);
        return false;
    }
    std::cout << "Checksum valid." << std::endl;
    return true;
}

QString CTraceCLI::calculateChecksum(const QString& filePath) {
    QFile file(filePath);
    
    if (!file.exists()) {
        QStringList possiblePaths;
        possiblePaths << QCoreApplication::applicationDirPath() + "/" + filePath
                     << "/usr/bin/" + filePath
                     << "/usr/local/bin/" + filePath;
                     
        for (const QString& path : possiblePaths) {
            file.setFileName(path);
            if (file.exists()) {
                break;
            }
        }
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    
    QCryptographicHash hash(QCryptographicHash::Sha256);
    if (hash.addData(&file)) {
        return hash.result().toHex();
    }
    
    return QString();
}

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