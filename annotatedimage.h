#ifndef ANNOTATEDIMAGE_H
#define ANNOTATEDIMAGE_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "project.h"

class AnnotatedImage : public QWidget
{
    Q_OBJECT
    QPixmap pixmap {};
    void setMargins();
    void enforceBoundryConditions();
    std::shared_ptr<Project>& currentProject;
    std::vector<Annotation> annotations {};
    QPointF mousePos {};
    QPointF imagePos {0, 0};
    float zoom {1};
    float imageScale {1};
    QRect target {};

public:
    void setImage(QString);
    explicit AnnotatedImage(std::shared_ptr<Project>& project, QWidget *parent = nullptr);

public slots:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

};

#endif // ANNOTATEDIMAGE_H
