#ifndef FILE_TREE_VIEW_HPP
#define FILE_TREE_VIEW_HPP

#include <QTreeView>
#include <QFileSystemModel>
#include <QWidget>

class FileTreeView : public QTreeView {
    Q_OBJECT

public:
    explicit FileTreeView(QWidget* parent = nullptr);
    void setRootPath(const QString& path);

signals:
    void fileSelected(const QString& filePath);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QFileSystemModel* model;
};

#endif // FILE_TREE_VIEW_HPP 