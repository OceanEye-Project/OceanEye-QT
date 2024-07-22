#ifndef EDITMEDIADIALOG_H
#define EDITMEDIADIALOG_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include "project.h"
#include "flowlayout.h"

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
    std::vector<int> selectedImages {};

    void updateImages();
    void removeSelected();
    void removeAll();
    void clearSelection();
    void refreshMedia();
    void setPreview(int idx);

    void showEvent(QShowEvent* event);

public:
    std::shared_ptr<Project>& currentProject;
    explicit EditMediaDialog(std::shared_ptr<Project>& project);

signals:
};


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


#endif // EDITMEDIADIALOG_H
