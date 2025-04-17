#ifndef IDE_HPP
#define IDE_HPP

#include <QObject>
#include "main_window.hpp"
#include "audit_service.hpp"
#include "project_manager.hpp"
#include "update_observer.hpp"

class UpdateService;

class IDE : public QObject, public UpdateObserver {
    Q_OBJECT

private:
    // Singleton instance
    static IDE* instance;
    
    // Components
    MainWindow* mainWindow;
    AuditService* auditService;
    ProjectManager* projectManager;
    UpdateService* updateService;
    
    // Private constructor for singleton
    IDE();
    
public:
    // Delete copy constructor and assignment operator
    IDE(const IDE&) = delete;
    IDE& operator=(const IDE&) = delete;
    
    // Destructor
    ~IDE();
    
    // Get singleton instance
    static IDE* getInstance();
    
    // Start and stop the IDE
    void start();
    void stop();
    
    // Get main window instance
    MainWindow* getMainWindow() const { return mainWindow; }
    
    // Observer method for updates
    virtual void onUpdateAvailable(const QString& newVersion, const QString& downloadUrl) override;
};

#endif // IDE_HPP 