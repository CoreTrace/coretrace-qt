#ifndef UPDATE_SERVICE_HPP
#define UPDATE_SERVICE_HPP

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QVector>
#include "update_observer.hpp"

class UpdateService : public QObject {
    Q_OBJECT

public:
    explicit UpdateService(QObject* parent = nullptr);
    ~UpdateService();

    // Observer pattern methods
    void addObserver(UpdateObserver* observer);
    void removeObserver(UpdateObserver* observer);
    
    // Check for updates
    void checkForUpdates();
    
    // Current version
    static QString getCurrentVersion();
    
    // Download and install update
    void downloadUpdate(const QString& url);

private slots:
    void onNetworkReply(QNetworkReply* reply);
    void onDownloadFinished(QNetworkReply* reply);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QNetworkAccessManager* networkManager;
    QVector<UpdateObserver*> observers;
    QString latestVersion;
    QString downloadUrl;
    QString downloadPath;
    
    void notifyObservers();
    void extractAndInstallUpdate();
    void restartApplication();
};

#endif // UPDATE_SERVICE_HPP 