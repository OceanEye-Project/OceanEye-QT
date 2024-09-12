#ifndef ANNOTATEDIMAGE_H
#define ANNOTATEDIMAGE_H

#include <QWidget>
#include <QPainter>
#include <QPushButton>
#include <QComboBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <algorithm>
#include "../util/project.h"
#include "../util/yolov8.h"
#include "qcombobox.h"

struct AnnotationHandle {
    // Where the handle is drawn, relative to the annotation box (0-1)
    QPointF point;
    // When the annotation is being modified
    // This value masks out which sides are modified by the handle
    bool T, L, B, R;
};

static const std::vector<AnnotationHandle> annotationHandles = {
    {{0.0, 0.0}, 1, 1, 0, 0},
    {{0.0, 0.5}, 0, 1, 0, 0},
    {{0.0, 1.0}, 0, 1, 1, 0},
    {{0.5, 1.0}, 0, 0, 1, 0},
    {{1.0, 1.0}, 0, 0, 1, 1},
    {{1.0, 0.5}, 0, 0, 0, 1},
    {{1.0, 0.0}, 1, 0, 0, 1},
    {{0.5, 0.0}, 1, 0, 0, 0}
};

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
    bool mouseWasPressed {false};
    std::shared_ptr<AnnotationHandle> selectedHandle {nullptr};
    int selectedAnnotation {-1};
    QTransform worldToImageTransform {};

    // void escape();
    // void deleteAnnotation();
    // void newAnnotation();
    // void changeClass();

public:
    
    QPushButton* annotationEditBtn;
    QPushButton* annotationNewBtn;
    QComboBox* annotationClassCombo;

    std::vector<Annotation> annotations {};
    void setImage();
    void setImage(QString);
    explicit AnnotatedImage(
        std::shared_ptr<Project>& project,
        QWidget *parent = nullptr
        );

    void triggerRepaint();


public slots:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

signals:
    void annotationsChanged();

};

#endif // ANNOTATEDIMAGE_H
