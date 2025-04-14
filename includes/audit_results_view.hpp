#pragma once

#include "ui_component.hpp"
#include "audit_result.hpp"
#include <QWidget>
#include <QList>

class AuditResultsView : public QWidget, public UIComponent {
    Q_OBJECT

public:
    explicit AuditResultsView(QWidget* parent = nullptr);
    void render() override;
    void displayResults(const QList<AuditResult>& results);

private:
    QList<AuditResult> currentResults;
}; 