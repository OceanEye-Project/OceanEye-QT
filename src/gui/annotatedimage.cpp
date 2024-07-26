#include "annotatedimage.h"

AnnotatedImage::AnnotatedImage(std::shared_ptr<Project>& project, QWidget *parent)
    : QWidget{parent}
    , currentProject(project)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
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

    painter.setBrush(Qt::transparent);

    painter.drawPixmap(0, 0, pixmap);

    QPen pen {Qt::red};
    pen.setWidth(3);
    painter.setPen(pen);

    auto handlesize = 15;
    auto imageMousePos = painter.combinedTransform().inverted().map(mousePos);

    for (auto& annotation : annotations) {
        pen.setColor(Qt::red);
        painter.setBrush(Qt::transparent);

        painter.drawRect(annotation.box);

        pen.setColor(Qt::black);

        
        for (auto& handle : annotationHandles) {
            if (selectedHandle && selectedHandle->point == handle.point && selectedAnnotation && selectedAnnotation->box == annotation.box) {
                painter.setBrush(Qt::red);
            } else {
                painter.setBrush(Qt::white);
            }

            auto point = annotation.box.topLeft() + QPointF(annotation.box.width() * handle.point.x(), annotation.box.height() * handle.point.y());
            painter.drawEllipse(point, handlesize, handlesize);

            // distance from the mouse to the handle
            // auto distance = (point - (mousePos - target.topLeft()) / imageScale).manhattanLength();
            // if (distance < handlesize * handlesize) {
            //     std::cout << "Selected handle" << mousePos.x() << " " << mousePos.y() << std::endl;
            // }
            auto distance = (point - imageMousePos).manhattanLength();
            if (distance * distance < handlesize * handlesize && mouseWasPressed) {
                selectedHandle = std::make_shared<AnnotationHandle>(handle);
                selectedAnnotation = std::make_shared<Annotation>(annotation);
            }
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
    mousePos = event->position();
    mouseWasPressed = true;
    repaint();
}

void AnnotatedImage::mouseReleaseEvent(QMouseEvent* event) {
    if (selectedHandle) {
        selectedHandle = nullptr;
    }
    if (selectedAnnotation) {
        selectedAnnotation = nullptr;
    }
    repaint();
}

void AnnotatedImage::mouseMoveEvent(QMouseEvent* event) {
    Qt::MouseButtons buttons = event->buttons();
    QPointF delta = event->position() - mousePos;

    if (buttons & Qt::LeftButton) {
        if (selectedHandle && selectedAnnotation) {
            std::cout << "Moving annotation" << std::endl;

            if (selectedHandle->T)
                selectedAnnotation->box.moveTop(delta.y());

            if (selectedHandle->L)
                selectedAnnotation->box.moveLeft(delta.x());

            if (selectedHandle->B)
                selectedAnnotation->box.moveBottom(delta.y());

            if (selectedHandle->R)
                selectedAnnotation->box.moveRight(delta.x());

            std::cout << "Moved annotation" << std::endl;


            // if (modifier.left()) {
            //     box.setLeft(std::max(0.0, box.left() + delta.x() / imageScale));
            // }
            // if (modifier.top()) {
            //     box.setTop(std::max(0.0, box.top() + delta.y() / imageScale));
            // }
            // if (modifier.right()) {
            //     box.setRight(std::min((double)pixmap.width(), box.right() + delta.x() / imageScale));
            // }
            // if (modifier.bottom()) {
            //     box.setBottom(std::min((double)pixmap.height(), box.bottom() + delta.y() / imageScale));
            // }

            // emit annotationsChanged();
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
