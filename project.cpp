#include "project.h"

Project::Project(const QString project_path) : settings(QDir::cleanPath(project_path + QDir::separator() + ".oceaneye.ini"), QSettings::Format::IniFormat) {

    if (settings.contains("modelPath"))
        loadModel(settings.value("modelPath").toString());

    if (settings.contains("modelConf"))
        model->modelScoreThreshold = settings.value("modelConf").toDouble();

    loadMedia();

}

std::vector<Annotation> Project::getAnnotation(const QString image_path) {
    std::vector<Annotation> annotations {};

    if (!settings.contains("annotations/" + image_path))
        return annotations;

    int size = settings.beginReadArray("annotations/" + image_path);

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);

        Annotation annotation {};

        annotation.classId = settings.value("classId").toInt();
        annotation.className = settings.value("className").toString();
        annotation.confidence = settings.value("confidence").toFloat();
        annotation.box.x = settings.value("x").toFloat();
        annotation.box.y = settings.value("y").toFloat();
        annotation.box.w = settings.value("w").toFloat();
        annotation.box.h = settings.value("h").toFloat();

        annotations.push_back(annotation);
    }
    settings.endArray();

    return annotations;
}

void Project::setAnnotation(const QString image_path, const std::vector<Annotation>& annotations) {

    settings.endArray();

    settings.beginWriteArray("annotations/" + image_path);
    settings.setValue("size", 0);

    for (int i=0; i<annotations.size(); i++) {
        auto annotation = annotations.at(i);

        settings.setArrayIndex(i);
        settings.setValue("classId", annotation.classId);
        settings.setValue("className", annotation.className);
        settings.setValue("confidence", annotation.confidence);
        settings.setValue("x", annotation.box.x);
        settings.setValue("y", annotation.box.y);
        settings.setValue("w", annotation.box.w);
        settings.setValue("h", annotation.box.h);

    }

    settings.endArray();
}

bool Project::isModelLoaded() {
    return model == nullptr;
}

void Project::loadModel(const QString modelPath) {
    model = std::make_unique<YOLOv8>(YOLOv8());

    model->modelPath = modelPath.toStdString();
    model->loadOnnxNetwork();
}

void Project::runDetection(const QString imagePath) {
    if (!isModelLoaded())
        return;

    cv::Mat img = cv::imread(imagePath.toStdString());

    auto annotations = model->runInference(img);

    setAnnotation(imagePath, annotations);
}

void Project::saveMedia() {
    settings.beginWriteArray("media");
    settings.setValue("size", 0);

    for (int i=0; i<media.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("path", media.at(i));
    }

    settings.endArray();
}

void Project::loadMedia() {
    media = {};

    int size = settings.beginReadArray("media");

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        media.push_back(settings.value("path").toString());
    }

    settings.endArray();
}


