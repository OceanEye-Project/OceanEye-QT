#ifndef PROJECT_H
#define PROJECT_H

#include <filesystem>
#include <vector>
#include <stdexcept>

#include <QSettings>
#include <QString>
#include <QDir>
#include <QListWidgetItem>
#include <QDebug>
#include <QString>
#include <QMessageBox>

#include "yolov8.h"
#include "settings.h"

class Project : public QObject
{
    Q_OBJECT

public:
    Project(QString project_path);
    Project(QString project_path, std::vector<QString> classes, QString project_name);

    QString projectPath;
    QString projectName;
    std::vector<Annotation> getAnnotation(const QString image_path);
    std::vector<QString> media {};
    std::unique_ptr<YOLOv8> model {nullptr};
    std::vector<QString> classes {};
    QList<QListWidgetItem*> selectedItems;

    void setAnnotation(const QString image_path, const std::vector<Annotation>& annotations);
    bool isModelLoaded();
    void loadModel(const QString modelPath);
    bool runDetection(const QString imagePath);
    void runSpecificDetection(const QString imagePath, const QList<QListWidgetItem *> classTypes);
    void loadMedia();
    void saveMedia();
    void setModelConf(int conf);
    void setClasses(std::vector<QString> classes);
    void setProjectName(QString newName);
    QSettings settings;

signals:
    void modelLoaded(QString modelPath);

private:
    void construct(QString project_path, std::vector<QString> annotationClasses, QString project_name);
};

#endif // PROJECT_H
