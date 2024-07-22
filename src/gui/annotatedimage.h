#ifndef ANNOTATEDIMAGE_H
#define ANNOTATEDIMAGE_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "../util/project.h"

class AnnotatedImage : public QWidget
{
    Q_OBJECT
    QPixmap pixmap {};
    void setMargins();
    void enforceBoundryConditions();
    std::shared_ptr<Project>& currentProject;
    QPointF mousePos {};
    QPointF imagePos {0, 0};
    float zoom {1};
    float imageScale {1};
    QRect target {};

public:
    std::vector<Annotation> annotations {};
    void setImage(QString);
    explicit AnnotatedImage(std::shared_ptr<Project>& project, QWidget *parent = nullptr);

public slots:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

signals:
    void annotationsChanged();

};

#endif // ANNOTATEDIMAGE_H
