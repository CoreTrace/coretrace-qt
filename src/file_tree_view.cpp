#include "../includes/file_tree_view.hpp"
#include <QHeaderView>
#include <QMouseEvent>
#include <QFileInfo>

FileTreeView::FileTreeView(QWidget* parent) : QTreeView(parent) {
    model = new QFileSystemModel(this);
    model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    
    setModel(model);
    setRootIsDecorated(true);
    setAnimated(true);
    setSortingEnabled(true);
    
    // Hide size and type columns
    hideColumn(1);
    hideColumn(2);
    hideColumn(3);
    
    // Set header to stretch
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    
    sortByColumn(0, Qt::AscendingOrder);
    
    // To make selection visible
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

void FileTreeView::setRootPath(const QString& path) {
    model->setRootPath(path);
    setRootIndex(model->index(path));
}

void FileTreeView::mouseReleaseEvent(QMouseEvent* event)
{
    QTreeView::mouseReleaseEvent(event);
    
    // Only process left clicks
    if (event->button() == Qt::LeftButton) {
        QModelIndex index = indexAt(event->pos());
        if (index.isValid()) {
            QString filePath = model->filePath(index);
            QFileInfo fileInfo(filePath);
            
            if (fileInfo.isFile()) {
                // Remember the current file
                currentFilePath = filePath;
                // Emit the signal
                emit fileSelected(filePath);
            }
        }
    }
}