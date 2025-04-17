#include "../includes/update_service.hpp"
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QProcess>
#include <QDebug>

UpdateService::UpdateService(QObject* parent) 
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
{
    connect(networkManager, &QNetworkAccessManager::finished, this, &UpdateService::onNetworkReply);
}

UpdateService::~UpdateService() {
    // Clean up observers
    observers.clear();
}

void UpdateService::addObserver(UpdateObserver* observer) {
    if (!observers.contains(observer)) {
        observers.append(observer);
    }
}

void UpdateService::removeObserver(UpdateObserver* observer) {
    observers.removeAll(observer);
}

void UpdateService::checkForUpdates() {
    // URL to your version check endpoint - replace with your actual API endpoint
    QUrl url("https://api.yourapp.com/version.json");
    
    QNetworkRequest request(url);
    networkManager->get(request);
}

QString UpdateService::getCurrentVersion() {
    // This should match your app's version in CMakeLists.txt or other configuration
    return "1.0.0"; 
}

void UpdateService::onNetworkReply(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject obj = doc.object();
            
            // Parse the version info from API
            if (obj.contains("version") && obj.contains("download_url")) {
                latestVersion = obj["version"].toString();
                downloadUrl = obj["download_url"].toString();
                
                // Compare versions
                if (latestVersion != getCurrentVersion()) {
                    qDebug() << "New version available:" << latestVersion;
                    notifyObservers();
                } else {
                    qDebug() << "Application is up to date";
                }
            }
        }
    } else {
        qDebug() << "Network error:" << reply->errorString();
    }
    
    reply->deleteLater();
}

void UpdateService::notifyObservers() {
    for (UpdateObserver* observer : observers) {
        observer->onUpdateAvailable(latestVersion, downloadUrl);
    }
}

void UpdateService::downloadUpdate(const QString& url) {
    QNetworkRequest request(url);
    QNetworkReply* reply = networkManager->get(request);
    
    // Disconnect previous connections to avoid multiple signals
    disconnect(networkManager, &QNetworkAccessManager::finished, this, &UpdateService::onNetworkReply);
    
    // Connect to download progress and finish signals
    connect(reply, &QNetworkReply::downloadProgress, this, &UpdateService::onDownloadProgress);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        this->onDownloadFinished(reply);
    });
    
    // Set download path in temp directory
    downloadPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + 
                  "/coretrace-update-" + latestVersion + ".zip";
}

void UpdateService::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    if (bytesTotal > 0) {
        int percentage = static_cast<int>((bytesReceived * 100) / bytesTotal);
        qDebug() << "Download progress:" << percentage << "%";
        // Could implement a progress dialog here
    }
}

void UpdateService::onDownloadFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QFile file(downloadPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            qDebug() << "Update downloaded to:" << downloadPath;
            
            // Extract and install the update
            extractAndInstallUpdate();
        } else {
            qDebug() << "Failed to save the update file:" << file.errorString();
        }
    } else {
        qDebug() << "Download failed:" << reply->errorString();
    }
    
    reply->deleteLater();
    
    // Reconnect the original handler
    connect(networkManager, &QNetworkAccessManager::finished, this, &UpdateService::onNetworkReply);
}

void UpdateService::extractAndInstallUpdate() {
    // This is a simplified implementation - adjust based on your actual deployment method
    
    QDir updateDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/coretrace-update");
    
    // Create update directory if it doesn't exist
    if (!updateDir.exists()) {
        updateDir.mkpath(".");
    }
    
    // Extract the downloaded zip file
    // Note: This is a simplified example. You might need a proper zip library
    QProcess extractProcess;
    extractProcess.setWorkingDirectory(updateDir.path());
    
#ifdef Q_OS_WIN
    extractProcess.start("powershell", QStringList() << "-command" << "Expand-Archive" << "-Path" << downloadPath << "-DestinationPath" << updateDir.path());
#else
    extractProcess.start("unzip", QStringList() << "-o" << downloadPath << "-d" << updateDir.path());
#endif
    
    if (extractProcess.waitForFinished()) {
        qDebug() << "Update extracted successfully";
        
        // Show confirmation dialog
        QMessageBox msgBox;
        msgBox.setWindowTitle("Update Ready");
        msgBox.setText("The update has been downloaded and is ready to install.");
        msgBox.setInformativeText("The application will restart to complete the installation. Continue?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        
        if (msgBox.exec() == QMessageBox::Yes) {
            // Copy files and restart
            restartApplication();
        }
    } else {
        qDebug() << "Failed to extract update:" << extractProcess.errorString();
        QMessageBox::critical(nullptr, "Update Error", "Failed to extract the update package.");
    }
}

void UpdateService::restartApplication() {
    // Script or batch file path for installation
    QString installerPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + 
                          "/coretrace-update/install_update.sh";
    
    // Create an installer script that copies files and restarts the app
    QFile installer(installerPath);
    if (installer.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&installer);
        
#ifdef Q_OS_WIN
        // Windows batch file
        out << "@echo off\n";
        out << "ping -n 2 127.0.0.1 > nul\n"; // Wait for 1 second
        out << "xcopy /E /Y \"" << QStandardPaths::writableLocation(QStandardPaths::TempLocation) << "\\coretrace-update\\*\" \"" << QApplication::applicationDirPath() << "\\\"\n";
        out << "start \"\" \"" << QApplication::applicationFilePath() << "\"\n";
#else
        // Unix shell script
        out << "#!/bin/bash\n";
        out << "sleep 1\n"; // Wait for 1 second
        out << "cp -rf " << QStandardPaths::writableLocation(QStandardPaths::TempLocation) << "/coretrace-update/* " << QApplication::applicationDirPath() << "/\n";
        out << QApplication::applicationFilePath() << " &\n";
#endif
        
        installer.close();
        
#ifndef Q_OS_WIN
        // Make the script executable on Unix
        QProcess::execute("chmod", QStringList() << "+x" << installerPath);
#endif
        
        // Start the installer and terminate the current instance
        QProcess::startDetached(installerPath);
        QApplication::quit();
    } else {
        qDebug() << "Failed to create installer script:" << installer.errorString();
        QMessageBox::critical(nullptr, "Update Error", "Failed to create update installer.");
    }
} 