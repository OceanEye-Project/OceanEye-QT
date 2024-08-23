#ifndef PROJECT_H
#define PROJECT_H

#include <filesystem>
#include <vector>

#include <QSettings>
#include <QString>
#include <QDir>
#include <QListWidgetItem>

#include "yolov8.h"

class Project : public QObject
{
    Q_OBJECT

public:
    Project(const QString project_path);

    QString projectPath;
    std::vector<Annotation> getAnnotation(const QString image_path);
    std::vector<QString> media {};
    std::unique_ptr<YOLOv8> model {nullptr};

    void setAnnotation(const QString image_path, const std::vector<Annotation>& annotations);
    bool isModelLoaded();
    void loadModel(const QString modelPath);
    bool runDetection(const QString imagePath);
    void runSpecificDetection(const QString imagePath, const QList<QListWidgetItem *> classTypes);
    void loadMedia();
    void saveMedia();
    void setModelConf(int conf);
    QSettings settings;

signals:
    void modelLoaded(QString modelPath);

};

#endif // PROJECT_H
