#include "annotatedimage.h"

// TODO better way to pass widgets through
AnnotatedImage::AnnotatedImage(
    std::shared_ptr<Project>& project,
    QWidget *parent
    )
    : QWidget{parent}
    , currentProject(project)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

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

    worldToImageTransform = painter.combinedTransform().inverted();

    painter.setBrush(Qt::transparent);

    painter.drawPixmap(0, 0, pixmap);

    QPen pen {};
    pen.setWidth(3);
    painter.setPen(pen);

    auto handlesize = 15;
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

        for (auto& handle : annotationHandles) {
            auto point = annotation.box.topLeft() + QPointF(annotation.box.width() * handle.point.x(), annotation.box.height() * handle.point.y());

            auto distance = (point - imageMousePos).manhattanLength();

            painter.setBrush(Qt::white);

            if (distance * distance < handlesize * handlesize) {
                // need to call redraw on mouse move for this to work :(
                // painter.setBrush(Qt::red);
                if (mouseWasPressed) {
                    selectedHandle = std::make_shared<AnnotationHandle>(handle);
                    selectedAnnotation = i;
                }
            }

            if (selectedHandle && selectedHandle->point == handle.point && selectedAnnotation == i) {
                painter.setBrush(QColor::fromHsv(hue, 245, 127, 255));
            }

            if (annotationEditBtn->isChecked())
                painter.drawEllipse(point, handlesize, handlesize);
        }
    }


    painter.restore();
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

    mousePos = event->position();
    if (annotationEditBtn->isChecked()) {
        mouseWasPressed = true;
    } else if (annotationNewBtn->isChecked()) {
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
        selectedHandle = std::make_unique<AnnotationHandle>(annotationHandles.at(4));

    } else if (annotationDeleteBtn->isChecked()) {

        QPointF imageMousePos = worldToImageTransform.map(mousePos);
        for (int i=0; i<annotations.size(); i++) {
            if (annotations.at(i).box.contains(imageMousePos.toPoint(), true)) {
                annotations.erase(annotations.begin() + i);
                emit annotationsChanged();
                break;
            }
        }
    }

    repaint();
}

void AnnotatedImage::mouseReleaseEvent(QMouseEvent* event) {
    if (selectedHandle) {
        selectedHandle = nullptr;
    }
    if (selectedAnnotation >= 0) {
        selectedAnnotation = -1;

        for (auto& annotation : annotations) {
            annotation.box = annotation.box.normalized();
        }

        emit annotationsChanged();
    }
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
