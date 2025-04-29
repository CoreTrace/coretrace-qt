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
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QFileSystemModel* model;
    QString currentFilePath;
};

#endif // FILE_TREE_VIEW_HPP