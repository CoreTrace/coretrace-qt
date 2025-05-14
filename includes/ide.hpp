#pragma once

#include "main_window.hpp"
#include "ui_component.hpp"
#include "audit_result.hpp"
#include "file_tree_view.hpp"
#include "cli_options_panel.hpp"
#include "audit_service.hpp"
#include "ctrace_cli.hpp"
#include "parser.hpp"
#include "audit_cache.hpp"
#include "error_highlighter.hpp"
#include "project_manager.hpp"
#include "project.hpp"
#include "audit_factory.hpp"
#include <QApplication>

class IDE {
public:
    static IDE* getInstance();
    void start();
    void stop();
    ProjectManager* getProjectManager() const { return projectManager; }
    MainWindow* getMainWindow() const { return mainWindow; }
    AuditService* getAuditService() const { return auditService; }

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