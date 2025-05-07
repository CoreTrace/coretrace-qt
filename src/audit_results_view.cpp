#include "../includes/audit_results_view.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

/**
 * @class AuditResultsView
 * @brief A widget for displaying audit results.
 * 
 * The AuditResultsView class provides a scrollable view to display
 * audit results, including file summaries and detailed issues.
 */

/**
 * @brief Constructs an AuditResultsView widget.
 * 
 * Initializes the layout for displaying audit results.
 * 
 * @param parent The parent widget (default is nullptr).
 */
AuditResultsView::AuditResultsView(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);
}

/**
 * @brief Renders the audit results in the view by dynamically creating and displaying widgets.
 * 
 * This method clears any existing widgets from the layout and then creates new widgets
 * to display the current audit results. Each result is represented by a summary label,
 * followed by a list of issues displayed as indented labels.
 */
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

/**
 * @brief Displays a list of audit results.
 * 
 * Updates the view with the provided audit results and triggers
 * a re-render to display the new data.
 * 
 * @param results A list of AuditResult objects to display.
 */
void AuditResultsView::displayResults(const QList<AuditResult>& results) {
    currentResults = results;
    render();
}