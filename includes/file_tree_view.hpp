#ifndef FILE_TREE_VIEW_HPP
#define FILE_TREE_VIEW_HPP

#include <QTreeView>
#include <QFileSystemModel>
#include <QWidget>
#include <QMenu>
#include <QAction>

class FileTreeView : public QTreeView {
    Q_OBJECT

public:
    explicit FileTreeView(QWidget* parent = nullptr);
    void setRootPath(const QString& path);

signals:
    void fileSelected(const QString& filePath);
    void fileCreated(const QString& filePath);
    void folderCreated(const QString& folderPath);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void createNewFile();
    void createNewFolder();
    void renameItem();
    void deleteItem();

private:
    QFileSystemModel* model;
    QString currentFilePath;
    QModelIndex contextMenuIndex;
    
    QMenu* createContextMenu();
    QString getUniqueName(const QString& basePath, const QString& name, const QString& extension = QString());
    bool createFileOrFolder(const QString& path, bool isFile);
};

#endif // FILE_TREE_VIEW_HPP