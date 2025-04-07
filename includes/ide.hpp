#pragma once

#include "main_window.hpp"
#include "audit_service.hpp"
#include "project_manager.hpp"
#include <QApplication>

class IDE {
public:
    static IDE* getInstance();
    void start();
    void stop();
    ProjectManager* getProjectManager() const { return projectManager; }
    MainWindow* getMainWindow() const { return mainWindow; }

private:
    IDE();
    ~IDE();
    IDE(const IDE&) = delete;
    IDE& operator=(const IDE&) = delete;

    static IDE* instance;
    MainWindow* mainWindow;
    AuditService* auditService;
    ProjectManager* projectManager;
}; 