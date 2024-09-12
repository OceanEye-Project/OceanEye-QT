#include "annotatedimage.h"

QTransform computeQTransform(const QRect &source, const QRect &target, const QPointF &translate, float zoom) {
    QTransform transform;
    transform.scale(1 / zoom, 1 / zoom);
    transform.translate(-translate.x(), -translate.y());
    transform.scale((double) target.width() / source.width(), (double) target.height() / source.height());
    transform.translate(-source.x(), -source.y());

    return transform;
}

const int handleSize = 4;

// TODO better way to pass widgets through
AnnotatedImage::AnnotatedImage(
    std::shared_ptr<Project>& project,
    QWidget *parent
    )
    : QWidget{parent}
    , currentProject(project)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setMouseTracking(true);

    // QShortcut *shortcutDelte = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    // connect(shortcutDelte, &QShortcut::activated, this, &AnnotatedImage::deleteAnnotation);
    connect(
        new QShortcut(QKeySequence(Qt::Key_Delete), this), 
        &QShortcut::activated, this, [this]() {
        deleteAnnotation();
        repaint();
    });

    // QShortcut *shortcutSpace = new QShortcut(QKeySequence(Qt::Key_Space), this);
    // connect(shortcutSpace, &QShortcut::activated, this, &AnnotatedImage::changeClass);
    connect(
        new QShortcut(QKeySequence(Qt::Key_Space), this), 
        &QShortcut::activated, this, [this]() {
        changeClass();
        repaint();
    });

    // QShortcut *shortcutN = new QShortcut(QKeySequence(Qt::Key_N), this);
    // connect(shortcutN, &QShortcut::activated, this, &AnnotatedImage::newAnnotation);
    connect(
        new QShortcut(QKeySequence(Qt::Key_N), this), 
        &QShortcut::activated, this, [this]() {
        annotationNewBtn->setChecked(true);
        repaint();
    });

    // QShortcut *shortcutEsc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    // connect(shortcutEsc, &QShortcut::activated, this, &AnnotatedImage::escape);
    connect(
        new QShortcut(QKeySequence(Qt::Key_Escape), this), 
        &QShortcut::activated, this, [this]() {
        annotationEditBtn->setChecked(true);

        if (selectedAnnotation >= 0) {
            selectedAnnotation = -1;
        }

        repaint();
    });

    connect(
        new QShortcut(QKeySequence(Qt::Key_R), this), 
        &QShortcut::activated, this, [this]() {
        zoom = 1;
        imagePos = {0, 0};
        setMargins();
        repaint();
    });
}


// void AnnotatedImage::escape() {
//     annotationEditBtn->setChecked(true);

//     if (selectedAnnotation >= 0) {
//         selectedAnnotation = -1;
//     }

//     repaint();
// }

// void AnnotatedImage::deleteAnnotation() {
//     if (selectedAnnotation >= 0) {
//         annotations.erase(annotations.begin() + selectedAnnotation);
//         selectedAnnotation = -1;
//         emit annotationsChanged();
//         repaint();
//     }
// }

// void AnnotatedImage::changeClass() {
//     if (selectedAnnotation >= 0) {
//         auto& annotation = annotations.at(selectedAnnotation);
//         annotation.classId = (annotation.classId + 1) % model_classes.size();
//         annotation.className = QString::fromStdString(model_classes.at(annotation.classId));
//         emit annotationsChanged();
//         repaint();
//     }
// }

// void AnnotatedImage::newAnnotation() {
//     annotationNewBtn->setChecked(true);
//     repaint();
// }

void AnnotatedImage::setImage() {
    pixmap = QPixmap();
    repaint();
}

void AnnotatedImage::triggerRepaint() {
    repaint();
}

void AnnotatedImage::setImage(QString path) {
    annotations = currentProject->getAnnotation(path);
    emit annotationsChanged();

    pixmap = QPixmap(path);
    zoom = 1;
    imagePos = {0, 0};
    setMargins();

    repaint();
}

void AnnotatedImage::resizeEvent(QResizeEvent* e) {
    setMargins();
}

void AnnotatedImage::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::black);

    painter.save();

    const QColor backgoundColor {127, 127, 127, 127};
    QBrush backgroundBrush {backgoundColor};
    backgroundBrush.setStyle(Qt::BrushStyle::CrossPattern);
    painter.setBrush(backgroundBrush);
    painter.drawRect(0, 0, width(), height());

    QRect source {
        0, 0, pixmap.width(), pixmap.height()
    };

    painter.setViewport(target);
    painter.setWindow(source);

    painter.translate(imagePos);
    painter.scale(zoom, zoom);

    worldToImageTransform = computeQTransform(target, source, imagePos, zoom);

    painter.setBrush(Qt::transparent);

    painter.drawPixmap(0, 0, pixmap);

    QPen pen {};
    pen.setWidth(1);
    painter.setPen(pen);

    auto imageMousePos = worldToImageTransform.map(mousePos);

    for (int i=0; i<annotations.size(); i++) {
        auto& annotation = annotations.at(i);

        int hue = (255 * annotation.classId) / model_classes.size();

        pen.setColor(QColor::fromHsv(hue, 245, 245, 255));
        painter.setPen(pen);

        painter.setBrush(Qt::transparent);

        painter.drawRect(annotation.box);

        pen.setColor(Qt::black);
        painter.setPen(pen);

        if (selectedAnnotation == i)  {
            for (auto& handle : annotationHandles) {
                auto point = annotation.box.topLeft() + QPointF(annotation.box.width() * handle.point.x(), annotation.box.height() * handle.point.y());

                painter.setBrush(Qt::white);

                if (selectedHandle && selectedHandle->point == handle.point && selectedAnnotation == i) {
                    painter.setBrush(QColor::fromHsv(hue, 245, 127, 255));
                }

                painter.drawEllipse(point, handleSize, handleSize);
                
            }
        }
    }

    painter.restore();

    if (annotationNewBtn->isChecked()) {
        painter.setPen(Qt::red);
        painter.drawLine(0, mousePos.y(), width(), mousePos.y());
        painter.drawLine(mousePos.x(), 0, mousePos.x(), height());
    }

    painter.setBrush(Qt::transparent);
    painter.setPen(Qt::black);
    painter.drawRect(0, 0, width(), height());

    mouseWasPressed = false;
}

void AnnotatedImage::enforceBoundryConditions() {
    float boundry = 50;

    if (imagePos.x() < (boundry - pixmap.width()) * zoom)
        imagePos.setX((boundry - pixmap.width()) * zoom);

    if (imagePos.x() > pixmap.width() - boundry)
        imagePos.setX(pixmap.width() - boundry);

    if (imagePos.y() < (boundry - pixmap.height()) * zoom)
        imagePos.setY((boundry - pixmap.height()) * zoom);

    if (imagePos.y() > pixmap.height() - boundry)
        imagePos.setY(pixmap.height() - boundry);

    float minZoom = 0.5;
    float maxZoom = 20;

    if (zoom > maxZoom)
        zoom = maxZoom;

    if (zoom < minZoom)
        zoom = minZoom;

}

void AnnotatedImage::mousePressEvent(QMouseEvent* event) {
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if (currentProject->media.empty())
        return;

    mousePos = event->position();

    if (annotationNewBtn->isChecked()) {
        QPointF imageMousePos = worldToImageTransform.map(mousePos);

        Annotation annotation {
            annotationClassCombo->currentData().toInt(),
            annotationClassCombo->currentText(),
            1.0f,
            {
                imageMousePos.toPoint(),
                QSize(1, 1)
            }
        };

        selectedAnnotation = annotations.size();
        annotations.push_back(annotation);

        // bottom right handle
        selectedHandle = std::make_shared<AnnotationHandle>(annotationHandles.at(4));

        emit annotationsChanged();
        repaint();

        return;
    } 

    QPointF imageMousePos = worldToImageTransform.map(mousePos);

    if (selectedAnnotation >= 0) {
        for (auto& handle : annotationHandles) {
            auto point = annotations.at(selectedAnnotation).box.topLeft() + QPointF(annotations.at(selectedAnnotation).box.width() * handle.point.x(), annotations.at(selectedAnnotation).box.height() * handle.point.y());
            auto distance = (point - imageMousePos).manhattanLength();
            if (distance * distance < handleSize * handleSize) {
                selectedHandle = std::make_shared<AnnotationHandle>(handle);
                return;
            }
        }
    }

    selectedAnnotation = -1;

    for (int i=0; i<annotations.size(); i++) {
        auto& annotation = annotations.at(i);
        if (annotation.box.contains(imageMousePos.toPoint(), true)) {
            selectedAnnotation = i;
            return;
        }
    }

    repaint();
}

void AnnotatedImage::mouseReleaseEvent(QMouseEvent* event) {
    if (selectedHandle) {
        selectedHandle = nullptr;
    }
    for (auto& annotation : annotations) {
        annotation.box = annotation.box.normalized();
    }

    emit annotationsChanged();

    repaint();
}

void AnnotatedImage::mouseMoveEvent(QMouseEvent* event) {
    Qt::MouseButtons buttons = event->buttons();
    QPointF delta = event->position() - mousePos;

    if (buttons & Qt::RightButton) {
        imagePos += delta / imageScale;
        enforceBoundryConditions();
        repaint();

    } else if (buttons & Qt::LeftButton) {
        if (selectedHandle && selectedAnnotation >= 0) {
            Annotation& annotation = annotations.at(selectedAnnotation);

            QPointF imageMousePos = worldToImageTransform.map(event->position());
            QPointF boundedMousePos = QPoint(
                std::clamp(imageMousePos.x(), 0.0, (double) pixmap.width()),
                std::clamp(imageMousePos.y(), 0.0, (double) pixmap.height())
            );

            if (selectedHandle->T)
                annotation.box.setTop(boundedMousePos.y());

            if (selectedHandle->L)
                annotation.box.setLeft(boundedMousePos.x());

            if (selectedHandle->B)
                annotation.box.setBottom(boundedMousePos.y());

            if (selectedHandle->R)
                annotation.box.setRight(boundedMousePos.x());

        } else {
            imagePos += delta / imageScale;
            enforceBoundryConditions();
        }

        repaint();
    }
    mousePos = event->position();
    repaint();
}

void AnnotatedImage::wheelEvent(QWheelEvent *event) {
    auto mousePt = (imagePos - (event->position() - target.topLeft()) / imageScale);
    auto zoomPt = mousePt / zoom;

    zoom *= std::pow(1.1, event->angleDelta().y() / 120.0);

    enforceBoundryConditions();

    auto newZoomPt = mousePt / zoom;

    imagePos += (zoomPt - newZoomPt) * zoom;

    enforceBoundryConditions();

    repaint();
}

void AnnotatedImage::setMargins() {
    if(pixmap.isNull()) return;

    int w = width();
    int h = height();
    int pixmapWidth = pixmap.width();
    int pixmapHeight = pixmap.height();

    if (w <= 0 || h <= 0)
        return;

    if (w * pixmapHeight > h * pixmapWidth) {
        int margin = w - (pixmapWidth * h / pixmapHeight);
        target = {margin / 2, 0, w - margin, h};
        imageScale = (float) h / pixmapHeight;
    } else {
        int margin = h - (pixmapHeight * w / pixmapWidth);
        target = {0, margin / 2, w, h - margin};
        imageScale = (float) w / pixmapWidth;
    }

}
