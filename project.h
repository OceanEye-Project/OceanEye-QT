#ifndef PROJECT_H
#define PROJECT_H

#include <filesystem>
#include <vector>

#include <QSettings>
#include <QString>
#include <QDir>

#include "yolov8.h"

class Project
{
    QSettings settings;
    std::unique_ptr<YOLOv8> model {nullptr};

public:
    Project(const QString project_path);

    std::vector<Annotation> getAnnotation(const QString image_path);
    std::vector<QString> media {};
    void setAnnotation(const QString image_path, const std::vector<Annotation>& annotations);

    bool isModelLoaded();
    void loadModel(const QString modelPath);
    void runDetection(const QString imagePath);
    void loadMedia();
    void saveMedia();
};

#endif // PROJECT_H
