#include "editmediadialog.h"
#include "./ui_editmedia.h"
#include <QApplication>
#include <QScreen>
#include <QScrollBar>

EditMediaDialog::EditMediaDialog(std::shared_ptr<Project>& project)
    : QWidget{}
    , ui(new Ui::EditMediaDialog)
    , currentProject(project)
{
    ui->setupUi(this);

    setWindowTitle("Edit Media");

    // Set window size
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width()*0.8;
    int screenHeight = screenGeometry.height()*0.8;
    resize(screenWidth, screenHeight);

    ui->scrollArea->resize(screenWidth*0.095, screenHeight);
    ui->mediaBox->resize(screenWidth*0.5, screenHeight);


    connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &EditMediaDialog::onScroll);
    connect(ui->removeSelected, &QPushButton::clicked, this, &EditMediaDialog::removeSelected);
    connect(ui->removeAll, &QPushButton::clicked, this, &EditMediaDialog::removeAll);
    connect(ui->clearSelection, &QPushButton::clicked, this, &EditMediaDialog::clearSelection);
    ui->mediaBox->setLayout(mediaLayout);

    if (!currentProject->media.empty())
        loadMoreMedia();
}

void EditMediaDialog::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    refreshMedia();
}

void EditMediaDialog::refreshMedia() {
    QLayoutItem* child;
    while ((child = mediaLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    selectedImages.clear();
    loadedItems = 0; // Reset loaded items

    loadMoreMedia(); // Load initial set of items
}

void EditMediaDialog::loadMoreMedia() {
    int size = 150;
    int start = loadedItems;
    int end = std::min(start + itemsPerLoad, static_cast<int>(currentProject->media.size()));

    for (int i = start; i < end; i++) {
            auto& file = currentProject->media.at(i);

        ImageThumbnail* thumbnail = new ImageThumbnail();
        thumbnail->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        thumbnail->setMinimumWidth(size);

        QPixmap pixmap{file};
        QPixmap scaledPixmap = pixmap.scaled(size, size, Qt::KeepAspectRatio);
        QIcon icon(scaledPixmap);
        thumbnail->setIcon(icon);
        thumbnail->setIconSize(scaledPixmap.size());

        connect(thumbnail, &ImageThumbnail::hover, this, [this, i] {
            setPreview(i);
        });
        connect(thumbnail, &ImageThumbnail::clicked, this, [this, i, thumbnail](bool checked) {
            selectedImages.insert({i, thumbnail});
        });

        mediaLayout->addWidget(thumbnail);
    }

    loadedItems = end; // Update the count of loaded items
}


void EditMediaDialog::setPreview(int idx) {
     QString path = currentProject->media.at(idx);
        previewPixmap.load(path);
        ui->mediaPreview->setPixmap(previewPixmap);

        ui->mediaPath->setText(path);
}

void EditMediaDialog::onScroll(int value) {
    QScrollBar* scrollbar = ui->scrollArea->verticalScrollBar();
    int maxScroll = scrollbar->maximum();
    int scrollPosition = scrollbar->value();
    int viewportHeight = ui->scrollArea->viewport()->height();

    if (scrollPosition + viewportHeight >= maxScroll - 100) { // Trigger load when close to the bottom
        if (loadedItems < static_cast<int>(currentProject->media.size())) {
            loadMoreMedia();
        }
    }
}


void EditMediaDialog::removeSelected() {
    for (auto it = selectedImages.begin(); it != selectedImages.end(); ++it) {
            currentProject->media.at(it->first) = currentProject->media.back();
            currentProject->media.pop_back();
        it->second->setParent(nullptr);
        delete it->second;
    }
    currentProject->saveMedia();
    selectedImages.clear();

    mediaLayout->heightForWidth(150);

    emit mediaChanged();
}

void EditMediaDialog::removeAll() {
    currentProject->media.clear();
    currentProject->saveMedia();
    selectedImages.clear();
    refreshMedia();

    emit mediaChanged();
}

void EditMediaDialog::clearSelection() {
    for (auto it = selectedImages.begin(); it != selectedImages.end(); ++it) {
        it->second->setChecked(false);
    }

    selectedImages.clear();
}
