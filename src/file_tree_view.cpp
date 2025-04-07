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
}

void FileTreeView::setRootPath(const QString& path) {
    model->setRootPath(path);
    setRootIndex(model->index(path));
}

void FileTreeView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QTreeView::mouseDoubleClickEvent(event);
    
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        QString filePath = model->filePath(index);
        QFileInfo fileInfo(filePath);
        
        if (fileInfo.isFile()) {
            emit fileSelected(filePath);
        }
    }
} 