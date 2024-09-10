#ifndef EDITMEDIADIALOG_H
#define EDITMEDIADIALOG_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include "../util/project.h"
#include "../gui/flowlayout.h"

class ImageThumbnail : public QPushButton {
    Q_OBJECT
public:
    ImageThumbnail() : QPushButton() {
        setAttribute(Qt::WA_Hover, true);
        setCheckable(true);
        setObjectName("thumbnail");
    }

private:
    void enterEvent(QEnterEvent* event) {
        emit hover();
        QPushButton::enterEvent(event);
    }

signals:
    void hover();

};

QT_BEGIN_NAMESPACE
namespace Ui {
class EditMediaDialog;
}
QT_END_NAMESPACE

class EditMediaDialog : public QWidget
{
    Q_OBJECT
    Ui::EditMediaDialog *ui;
    FlowLayout* mediaLayout = new FlowLayout();
    QPixmap previewPixmap {};
    std::map<int, ImageThumbnail*> selectedImages {};
    
    void removeSelected();
    void removeAll();
    void clearSelection();
    void refreshMedia();
    void setPreview(int idx);
    void showEvent(QShowEvent* event);
    void loadMoreMedia();

    int loadedItems; // Tracks the number of currently loaded items
    const int itemsPerLoad = 20; // Number of items to load at once

public:
    std::shared_ptr<Project>& currentProject;
    explicit EditMediaDialog(std::shared_ptr<Project>& project);

public slots:
    void onScroll(int value);

signals:
    void mediaChanged();
};


#endif // EDITMEDIADIALOG_H
