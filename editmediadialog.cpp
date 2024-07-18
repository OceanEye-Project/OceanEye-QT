#include "editmediadialog.h"
#include "./ui_editmedia.h"

EditMediaDialog::EditMediaDialog(std::shared_ptr<Project>& project)
    : QWidget{}
    , ui(new Ui::EditMediaDialog)
    , currentProject(project)
{
    ui->setupUi(this);

    setWindowTitle("Edit Media");

    // connect(ui->addMedia, &QPushButton::clicked, currentProject.get(), &Project::addMedia);
    connect(ui->removeSelected, &QPushButton::clicked, this, &EditMediaDialog::removeSelected);
    connect(ui->removeAll, &QPushButton::clicked, this, &EditMediaDialog::removeAll);
    connect(ui->clearSelection, &QPushButton::clicked, this, &EditMediaDialog::clearSelection);
    ui->mediaBox->setLayout(mediaLayout);

    setPreview(0);
}

void EditMediaDialog::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    refreshMedia();
}

void EditMediaDialog::refreshMedia() {
    QLayoutItem *wItem;
    while ((wItem = mediaLayout->takeAt(0)) != 0)
        delete wItem;

    int size = 150;
    for (int i=0; i<currentProject->media.size(); i++) {
        auto& file = currentProject->media.at(i);

        ImageThumbnail* thumbnail = new ImageThumbnail();

        thumbnail->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        // thumbnail->setMinimumHeight(size);
        thumbnail->setMinimumWidth(size);

        QPixmap pixmap {file};
        QPixmap scaledPixmap = pixmap.scaled(size, size, Qt::KeepAspectRatio);
        QIcon icon(scaledPixmap);
        thumbnail->setIcon(icon);
        thumbnail->setIconSize(scaledPixmap.size());

        connect(thumbnail, &ImageThumbnail::hover, this, [this, i] {
            setPreview(i);
        });
        mediaLayout->addWidget(thumbnail);
    }

    mediaLayout->heightForWidth(size);

}
void EditMediaDialog::setPreview(int idx) {
    QString path = currentProject->media.at(idx);
    previewPixmap.load(path);
    ui->mediaPreview->setPixmap(previewPixmap);

    ui->mediaPath->setText(path);
}

void EditMediaDialog::updateImages() {

}

void EditMediaDialog::removeSelected() {

}

void EditMediaDialog::removeAll() {

}

void EditMediaDialog::clearSelection() {

}
