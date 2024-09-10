#include "project.h"
#include <QDebug>
#include <QString>

Project::Project(const QString project_path)
    : settings(QDir::cleanPath(project_path + QDir::separator() + ".oceaneye.ini")
    , QSettings::Format::IniFormat)
    , projectPath(project_path) {

    loadModel(settings.value("Model Path").toString());

    if (settings.contains("Model Confidence"))
        setModelConf(settings.value("Model Confidence").toInt());

    loadMedia();

}

std::vector<Annotation> Project::getAnnotation(const QString image_path) {
    std::vector<Annotation> annotations {};

    // if (!settings.contains("annotations/" + image_path))
        // return annotations;

    int size = settings.beginReadArray("annotations/" + image_path);

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);

        Annotation annotation {};

        annotation.classId = settings.value("classId").toInt();
        annotation.className = settings.value("className").toString();
        annotation.confidence = settings.value("confidence").toFloat();
        annotation.box.setRect(
            settings.value("x").toFloat(),
            settings.value("y").toFloat(),
            settings.value("w").toFloat(),
            settings.value("h").toFloat()
        );

        annotations.push_back(annotation);
    }
    settings.endArray();

    return annotations;
}

void Project::setAnnotation(const QString image_path, const std::vector<Annotation>& annotations) {

    settings.beginWriteArray("annotations/" + image_path);
    settings.remove("");

    for (int i=0; i<annotations.size(); i++) {
        auto annotation = annotations.at(i);

        settings.setArrayIndex(i);
        settings.setValue("classId", annotation.classId);
        settings.setValue("className", annotation.className);
        settings.setValue("confidence", annotation.confidence);
        settings.setValue("x", annotation.box.x());
        settings.setValue("y", annotation.box.y());
        settings.setValue("w", annotation.box.width());
        settings.setValue("h", annotation.box.height());

    }

    settings.endArray();
}

void Project::setModelConf(int conf) {
    if (isModelLoaded())
        model->modelScoreThreshold = (float) conf / 100.0f;

    settings.setValue("Model Confidence", conf);
}

bool Project::isModelLoaded() {
    if (model) return true;
    return false;
}

void Project::loadModel(const QString modelPath) {
    if (modelPath.isEmpty())
        return;

    model = std::make_unique<YOLOv8>(YOLOv8());

    model->modelPath = modelPath.toStdString();
    model->loadOnnxNetwork();

    model->modelScoreThreshold = settings.value("Model Confidence").toInt() / 100.0f;

    settings.setValue("Model Path", modelPath);

    emit modelLoaded(modelPath);
}

// Returns true if there are more than 0 annotations, false otherwise
bool Project::runDetection(const QString imagePath) {
    if (!isModelLoaded()) {
        qWarning() << "No Model Loaded!";
        return false;
    }

    cv::Mat img = cv::imread(imagePath.toStdString());

    auto annotations = model->runInference(img);

    if (annotations.size() > 0) {
        setAnnotation(imagePath, annotations);
        return true;
    }

    return false;
}

void Project::runSpecificDetection(const QString imagePath, const QList<QListWidgetItem *> classTypes) {
    std::__1::vector<Annotation> specificAnnotations = {};
    std::set<QString> classTypesSet = {};
    if (!isModelLoaded()) {
        qWarning() << "Attempted detection without model loaded.";
        return;
    }
    qInfo() << "Running Detection on: " << imagePath;

    cv::Mat img = cv::imread(imagePath.toStdString());

    auto annotations = model->runInference(img);

    for (auto classType: classTypes) {
        QString className = classType->text();
        classTypesSet.insert(className);
    }

    for (auto annotation : annotations) {
        if (classTypesSet.find(annotation.className) != classTypesSet.end()) {
            specificAnnotations.push_back(annotation);
        }
    }

    setAnnotation(imagePath, specificAnnotations);
}

void Project::saveMedia() {
    settings.beginWriteArray("media");
    settings.setValue("size", 0);
    settings.remove("");
    int count = 0;

    for (int i=0; i<media.size(); i++) {
        ++count;
        qInfo() << "Saving media: " << media.at(i);
        settings.setArrayIndex(i);
        settings.setValue("path", media.at(i));
    }
    
    qInfo() << "Done saving media. Saved " << count << " items";
    settings.endArray();
}

void Project::loadMedia() {
    media = {};
    int count = 0;
    int size = settings.beginReadArray("media");

    for (int i = 0; i < size; ++i) {
        ++count;
        settings.setArrayIndex(i);
        media.push_back(settings.value("path").toString());
        qInfo() << "Loading Media: " << media.at(i);
    }
    qInfo() << "Done loading media. Loaded " << count << " items";
    settings.endArray();
}


