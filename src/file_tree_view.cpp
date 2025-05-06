#include "../includes/file_tree_view.hpp"
#include <QHeaderView>
#include <QMouseEvent>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDebug>

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

void FileTreeView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    contextMenuIndex = index;
    
    QMenu* menu = createContextMenu();
    menu->exec(event->globalPos());
    delete menu;
}

QMenu* FileTreeView::createContextMenu()
{
    QMenu* menu = new QMenu(this);
    
    // Get the path of the item under the cursor
    QString path;
    if (contextMenuIndex.isValid()) {
        path = model->filePath(contextMenuIndex);
    } else {
        // If no item is selected, use the root path
        path = model->rootPath();
    }
    
    QFileInfo fileInfo(path);
    
    // Add actions based on whether it's a file or directory
    if (fileInfo.isDir()) {
        // For directories, add "New File" and "New Folder" options
        QAction* newFileAction = menu->addAction("New File");
        connect(newFileAction, &QAction::triggered, this, &FileTreeView::createNewFile);
        
        QAction* newFolderAction = menu->addAction("New Folder");
        connect(newFolderAction, &QAction::triggered, this, &FileTreeView::createNewFolder);
        
        menu->addSeparator();
    }
    
    // Add rename and delete options for both files and folders
    QAction* renameAction = menu->addAction("Rename");
    connect(renameAction, &QAction::triggered, this, &FileTreeView::renameItem);
    
    QAction* deleteAction = menu->addAction("Delete");
    connect(deleteAction, &QAction::triggered, this, &FileTreeView::deleteItem);
    
    return menu;
}

void FileTreeView::createNewFile()
{
    QString dirPath;
    if (contextMenuIndex.isValid()) {
        dirPath = model->filePath(contextMenuIndex);
        QFileInfo fileInfo(dirPath);
        // If the selected item is a file, use its parent directory
        if (fileInfo.isFile()) {
            dirPath = fileInfo.dir().path();
        }
    } else {
        // If no item is selected, use the root path
        dirPath = model->rootPath();
    }
    
    bool ok;
    QString fileName = QInputDialog::getText(this, 
                                           "New File", 
                                           "Enter file name:", 
                                           QLineEdit::Normal, 
                                           "untitled.txt", 
                                           &ok);
    
    if (ok && !fileName.isEmpty()) {
        QString filePath = getUniqueName(dirPath, fileName);
        if (createFileOrFolder(filePath, true)) {
            // Refresh the model
            QString rootPath = model->rootPath();
            model->setRootPath("");
            model->setRootPath(rootPath);
            emit fileCreated(filePath);
        }
    }
}

void FileTreeView::createNewFolder()
{
    QString dirPath;
    if (contextMenuIndex.isValid()) {
        dirPath = model->filePath(contextMenuIndex);
        QFileInfo fileInfo(dirPath);
        // If the selected item is a file, use its parent directory
        if (fileInfo.isFile()) {
            dirPath = fileInfo.dir().path();
        }
    } else {
        // If no item is selected, use the root path
        dirPath = model->rootPath();
    }
    
    bool ok;
    QString folderName = QInputDialog::getText(this, 
                                             "New Folder", 
                                             "Enter folder name:", 
                                             QLineEdit::Normal, 
                                             "New Folder", 
                                             &ok);
    
    if (ok && !folderName.isEmpty()) {
        QString folderPath = getUniqueName(dirPath, folderName);
        if (createFileOrFolder(folderPath, false)) {
            // Refresh the model
            QString rootPath = model->rootPath();
            model->setRootPath("");
            model->setRootPath(rootPath);
            emit folderCreated(folderPath);
        }
    }
}

void FileTreeView::renameItem()
{
    if (!contextMenuIndex.isValid()) {
        return;
    }
    
    QString oldPath = model->filePath(contextMenuIndex);
    QFileInfo fileInfo(oldPath);
    
    bool ok;
    QString newName = QInputDialog::getText(this, "Rename", 
                                          "Enter new name:", 
                                          QLineEdit::Normal, 
                                          fileInfo.fileName(), 
                                          &ok);
    
    if (ok && !newName.isEmpty()) {
        QString newPath = fileInfo.dir().filePath(newName);
        
        if (QFile::exists(newPath)) {
            QMessageBox::warning(this, "Rename Failed", 
                               "A file or folder with that name already exists.");
            return;
        }
        
        if (QFile::rename(oldPath, newPath)) {
            // Refresh the model by resetting the root path
            QString rootPath = model->rootPath();
            model->setRootPath("");
            model->setRootPath(rootPath);
        } else {
            QMessageBox::warning(this, "Rename Failed", 
                               "Failed to rename the item.");
        }
    }
}

void FileTreeView::deleteItem()
{
    if (!contextMenuIndex.isValid()) {
        return;
    }
    
    QString path = model->filePath(contextMenuIndex);
    QFileInfo fileInfo(path);
    
    QString itemType = fileInfo.isDir() ? "folder" : "file";
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
                                                            "Confirm Delete", 
                                                            QString("Are you sure you want to delete this %1?").arg(itemType),
                                                            QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        bool success = false;
        
        if (fileInfo.isDir()) {
            QDir dir(path);
            success = dir.removeRecursively();
        } else {
            success = QFile::remove(path);
        }
        
        if (success) {
            // Refresh the model by resetting the root path
            QString rootPath = model->rootPath();
            model->setRootPath("");
            model->setRootPath(rootPath);
        } else {
            QMessageBox::warning(this, "Delete Failed", 
                               QString("Failed to delete the %1.").arg(itemType));
        }
    }
}

QString FileTreeView::getUniqueName(const QString& basePath, const QString& name, const QString& extension)
{
    QString fullName = name;
    if (!extension.isEmpty() && !fullName.endsWith(extension)) {
        fullName += extension;
    }
    
    QString path = QDir(basePath).filePath(fullName);
    QFileInfo fileInfo(path);
    
    // If the file doesn't exist, return the original path
    if (!fileInfo.exists()) {
        return path;
    }
    
    // Otherwise, find a unique name by adding a number
    QString baseName = fileInfo.baseName();
    QString ext = fileInfo.completeSuffix();
    if (!ext.isEmpty()) {
        ext = "." + ext;
    }
    
    int counter = 1;
    QString newPath;
    
    do {
        newPath = QDir(basePath).filePath(baseName + QString(" (%1)").arg(counter) + ext);
        counter++;
    } while (QFileInfo(newPath).exists());
    
    return newPath;
}

bool FileTreeView::createFileOrFolder(const QString& path, bool isFile)
{
    if (isFile) {
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            return true;
        }
    } else {
        QDir dir;
        if (dir.mkpath(path)) {
            return true;
        }
    }
    
    QMessageBox::warning(this, "Creation Failed", 
                        QString("Failed to create the %1.").arg(isFile ? "file" : "folder"));
    return false;
}