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
    int mediaSize = static_cast<int>(currentProject->media.size());
    
    // Ensure that 'start' is within the valid range
    int end = std::min(start + itemsPerLoad, mediaSize);

    for (int i = start; i < end; i++) {
        auto& file = currentProject->media.at(i);

        QPixmap pixmap;
        bool loaded = pixmap.load(file);  // Attempt to load the pixmap

        if (!loaded || pixmap.isNull()) {  // Check if the pixmap is valid
            qWarning() << "Failed to load image from:" << file;  // Log a warning
            continue;  // Skip this iteration if the pixmap is invalid
        }

        ImageThumbnail* thumbnail = new ImageThumbnail();
        thumbnail->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        thumbnail->setMinimumWidth(size);

        QPixmap scaledPixmap = pixmap.scaled(size, size, Qt::KeepAspectRatio);
        QIcon icon(scaledPixmap);
        thumbnail->setIcon(icon);
        thumbnail->setIconSize(scaledPixmap.size());

        connect(thumbnail, &ImageThumbnail::hover, this, [this, i] {
            if (i < currentProject->media.size()) {
                setPreview(i);
            } else {
                qWarning() << "Attempt to set preview for out-of-range index" << i;
            }
        });
        connect(thumbnail, &ImageThumbnail::clicked, this, [this, i, thumbnail](bool checked) {
            selectedImages.insert({i, thumbnail});
        });

        mediaLayout->addWidget(thumbnail);
    }

    loadedItems = end; // Update the count of loaded items
}

void EditMediaDialog::setPreview(int idx) {
    if (idx < 0 || idx >= currentProject->media.size()) {
        qWarning() << "Invalid media index:" << idx;
        return;
    }
    QString path = currentProject->media.at(idx);
    bool loaded = previewPixmap.load(path);
    
    if (!loaded || previewPixmap.isNull()) {
        return;
    }

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
    // Create a temporary list to store the indices to be deleted
    std::vector<int> indicesToDelete;

    // Loop through the selected images and store indices
    for (auto it = selectedImages.begin(); it != selectedImages.end(); ++it) {
        // Ensure the index is within bounds before accessing the vector
        if (it->first >= 0 && it->first < currentProject->media.size()) {
            QString imageFilename = currentProject->media.at(it->first);
            QString annotationFilename = imageFilename + ".yaml";

            try {
                deleteImageAndAnnotation(imageFilename, annotationFilename);
            } catch(...) {
                qWarning() << "Function deleteImageAndAnnotation failed to delete image or annotation.";
            }

            indicesToDelete.push_back(it->first);  // Collect indices to be deleted
            it->second->setParent(nullptr);
            delete it->second;
        } else {
            qWarning() << "Index out of bounds: " << it->first;
        }
    }

    // Now delete the items from the media vector, in reverse order to avoid invalidating indices
    std::sort(indicesToDelete.rbegin(), indicesToDelete.rend());  // Sort in reverse order
    for (int index : indicesToDelete) {
        if (index >= 0 && index < currentProject->media.size()) {
            currentProject->media.erase(currentProject->media.begin() + index);  // Remove the media item
        }
    }
    currentProject->saveMedia();
    selectedImages.clear();
    mediaLayout->heightForWidth(150);
    refreshMedia();
    emit mediaChanged();
}

void EditMediaDialog::removeAll() {
    for(auto imageFilename: currentProject->media) {
        QString annotationFilename = imageFilename + ".yaml";
        try {
            deleteImageAndAnnotation(imageFilename, annotationFilename);
        } catch(...) {
            qWarning() << "Function deleteImageAndAnnotation in editmediadialog.cpp failed to delete image or annotation.";
        }
    }
    currentProject->media.clear();
    currentProject->saveMedia();
    selectedImages.clear();
    refreshMedia();

    emit mediaChanged();
}

void EditMediaDialog::deleteImageAndAnnotation(QString imageFilename, QString annotationFilename) {
    // Attempt to delete the image
    if (QFile::exists(imageFilename)) {
        if (QFile::remove(imageFilename)) {
            qDebug() << "Deleted image file: " << imageFilename;
        } else {
            qDebug() << "Failed to delete image file: " << imageFilename;
        }
    } else {
        qDebug() << "Image file does not exist: " << imageFilename;
    }

    // Attempt to delete the annotation file
    if (QFile::exists(annotationFilename)) {
        if (QFile::remove(annotationFilename)) {
            qDebug() << "Deleted annotation file: " << annotationFilename;
        } else {
            qDebug() << "Failed to delete annotation file: " << annotationFilename;
        }
    } else {
        qDebug() << "Annotation file does not exist: " << annotationFilename;
    }
}

void EditMediaDialog::clearSelection() {
    for (auto it = selectedImages.begin(); it != selectedImages.end(); ++it) {
        it->second->setChecked(false);
    }

    selectedImages.clear();
}
