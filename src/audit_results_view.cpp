#include "../includes/audit_results_view.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

AuditResultsView::AuditResultsView(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);
}

void AuditResultsView::render() {
    // Clear existing widgets
    QLayout* layout = this->layout();
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Create new widgets for each result
    for (const AuditResult& result : currentResults) {
        QLabel* summaryLabel = new QLabel(result.getSummary(), this);
        layout->addWidget(summaryLabel);

        for (const QString& issue : result.getIssues()) {
            QLabel* issueLabel = new QLabel("  • " + issue, this);
            layout->addWidget(issueLabel);
        }
    }
}

void AuditResultsView::displayResults(const QList<AuditResult>& results) {
    currentResults = results;
    render();
} 