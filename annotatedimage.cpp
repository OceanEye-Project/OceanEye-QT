#include "annotatedimage.h"

AnnotatedImage::AnnotatedImage(std::shared_ptr<Project>& project, QWidget *parent)
    : QWidget{parent}
    , currentProject(project)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}


void AnnotatedImage::setImage(QString path) {
    annotations = currentProject->getAnnotation(path);

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

    QPen pen {};
    QBrush brush {};

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect source {
        0, 0, pixmap.width(), pixmap.height()
    };

    painter.save();

    painter.setViewport(target);
    painter.setWindow(source);

    painter.translate(imagePos);
    painter.scale(zoom, zoom);

    painter.drawPixmap(0, 0, pixmap);

    for (auto& annotation : annotations) {
        painter.drawRect(annotation.box);
    }

    painter.restore();

    painter.drawRect(0, 0, width(), height());
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
}
void AnnotatedImage::mouseMoveEvent(QMouseEvent* event) {
    QPointF newPos = event->position();
    QPointF delta = newPos - mousePos;
    imagePos += delta / imageScale;
    mousePos = newPos;

    enforceBoundryConditions();

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
