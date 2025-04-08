#include "project.h"

// TODO handle missing images

Project::Project(QString project_path) : 
    settings(
        QDir::cleanPath(project_path + QDir::separator() + "oceaneye_settings.yaml"),
        registerYAMLFormat()
    )
{
    classes = {};
    int count = 0;
    int size = settings.beginReadArray("classes");

    for (int i = 0; i < size; ++i) {
        ++count;
        settings.setArrayIndex(i);
        classes.push_back(settings.value("name").toString());
        qInfo() << "Loading Class: " << classes.at(i);
    }
    qInfo() << "Done loading classes. Loaded " << count << " items";
    settings.endArray();

    projectName = settings.value("Project Name").toString();

    construct(project_path, classes, projectName);
}

Project::Project(QString project_path, std::vector<QString> classes, QString project_name) :
    settings(
        QDir::cleanPath(project_path + QDir::separator() + "oceaneye_settings.yaml"),
        registerYAMLFormat()
    )
{
    construct(project_path, classes, project_name);
}

void Project::construct(QString project_path, std::vector<QString> annotationClasses, QString project_name) {
    projectPath = project_path;

    QDir projectDir(projectPath);
    projectDir.mkdir("annotations");

    settings.beginWriteArray("classes");
    settings.setValue("size", 0);
    settings.remove("");
    int count = 0;

    for (int i=0; i<annotationClasses.size(); i++) {
        ++count;
        settings.setArrayIndex(i);
        settings.setValue("name", annotationClasses.at(i));
    }
    settings.endArray();

    classes = annotationClasses;

    settings.setValue("Project Name", project_name);
    projectName = project_name;

    loadModel(settings.value("Model Path").toString());

    if (settings.contains("Model Confidence"))
        setModelConf(settings.value("Model Confidence").toInt());

    loadMedia();
}

std::vector<Annotation> Project::getAnnotation(const QString image_path) {
    std::vector<Annotation> annotations {};

    const QFileInfo file_info(image_path);

    QDir projectDir(projectPath);
    projectDir.mkdir("annotations");
    projectDir.cd("annotations");
    
    QSettings image_settings(
        projectDir.absoluteFilePath(file_info.fileName() + ".yaml"),
        registerYAMLFormat()
    );

    int size = image_settings.beginReadArray("annotations");

    for (int i = 0; i < size; ++i) {
        image_settings.setArrayIndex(i);

        Annotation annotation {};
        
        qInfo() << "Loading annotation: #" << i;
        qInfo() << "=============================";
        qInfo() << "Class ID: " << image_settings.value("classId").toInt();
        qInfo() << "Class Name: " << image_settings.value("className");
        qInfo() << "Confidence: " << image_settings.value("confidence").toFloat() << "%";
        qInfo() << "Annotation # " << i << " complete";
        qInfo() << "=============================";

        try{
        annotation.classId = image_settings.value("classId").toInt();
        annotation.className = image_settings.value("className").toString();
        annotation.confidence = image_settings.value("confidence").toFloat();  
        

        annotation.box.setRect(
            image_settings.value("x").toFloat(),
            image_settings.value("y").toFloat(),
            image_settings.value("w").toFloat(),
            image_settings.value("h").toFloat()
        );

        annotations.push_back(annotation);
        }catch(const std::exception &exec){
            qInfo() << "Error with project loading annotation.";
            qFatal() << "Error is: " << exec.what();
            qErrnoWarning("Error with project loading annotation. Check error logs for more information");
        }
        
    }
    image_settings.endArray();

    return annotations;
}

void Project::setAnnotation(const QString image_path, const std::vector<Annotation>& annotations) {
    const QFileInfo file_info(image_path);

    QDir projectDir(projectPath);
    projectDir.mkdir("annotations");
    projectDir.cd("annotations");

    QSettings image_settings(
        projectDir.absoluteFilePath(file_info.fileName() + ".yaml"),
        registerYAMLFormat()
    );

    image_settings.beginWriteArray("annotations");
    image_settings.remove("");

    for (int i=0; i<annotations.size(); i++) {
        auto annotation = annotations.at(i);

        image_settings.setArrayIndex(i);
        image_settings.setValue("classId", annotation.classId);
        image_settings.setValue("className", annotation.className);
        image_settings.setValue("confidence", annotation.confidence);
        image_settings.setValue("x", annotation.box.x());
        image_settings.setValue("y", annotation.box.y());
        image_settings.setValue("w", annotation.box.width());
        image_settings.setValue("h", annotation.box.height());

    }

    image_settings.endArray();
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

    std::vector<QString> classes = YOLOv8::loadClasses(modelPath.toStdString());

    if (!std::equal(classes.begin(), classes.end(), this->classes.begin())) {
        QMessageBox msgBox;
        QString info = "Model classes do not match project classes";
        info += "\n\nProject Classes:\n";
        for (auto &c : this->classes) {
            info += "- " + c + "\n";
        }
        info += "\nModel Classes:\n";
        for (auto &c : classes) {
            info += "- " + c + "\n";
        }
        msgBox.setText(info);
        msgBox.exec();
        return;
    }

    model = std::make_unique<YOLOv8>(YOLOv8());

    try{
        model->modelPath = modelPath.toStdString();
        model->loadOnnxNetwork();

        model->modelScoreThreshold = settings.value("Model Confidence").toInt() / 100.0f;

        settings.setValue("Model Path", modelPath);

        emit modelLoaded(modelPath);

    }catch(const std::exception &exec){
        qInfo() << "Error with project loading model.";
        qFatal() << "Error is: " << exec.what();
        qErrnoWarning("Error with project loading model. Check error logs for more information");
    }

   
}

// Returns true if there are more than 0 annotations, false otherwise
bool Project::runDetection(const QString imagePath) {
    try{
        if (!isModelLoaded()) {
        qWarning() << "No Model Loaded!";
        return false;
        }

        cv::Mat img = cv::imread(imagePath.toStdString());

        auto annotations = model->runInference(img, classes);

        if (annotations.size() > 0) {
            setAnnotation(imagePath, annotations);
            return true;
        }
    }catch(const std::exception &exec){
        qInfo() << "Error running upload detection.";
        qFatal() << "Error is: " << exec.what();
        qErrnoWarning("Error running upload detection. Check error logs for more information");
    }
    return false;
}

void Project::runSpecificDetection(const QString imagePath, const QList<QListWidgetItem *> classTypes) {
    try{
        std::vector<Annotation> specificAnnotations = {};
        std::set<QString> classTypesSet = {};
        if (!isModelLoaded()) {
            qWarning() << "Attempted detection without model loaded.";
            return;
        }
        qInfo() << "Running Detection on: " << imagePath;

        cv::Mat img = cv::imread(imagePath.toStdString());

        auto annotations = model->runInference(img, classes);

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

    }catch(const std::exception &exec){
        qInfo() << "Error running specific detection.";
        qFatal() << "Error is: " << exec.what();
        qErrnoWarning("Error running specifc detection. Check error logs for more information");
    }
}

void Project::saveMedia() {
    try{
        settings.beginWriteArray("media");
        settings.setValue("size", 0);
        settings.remove("");
        int count = 0;

        for (int i=0; i<media.size(); i++) {
            ++count;
            // qInfo() << "Saving media: " << media.at(i);
            settings.setArrayIndex(i);
            settings.setValue("path", media.at(i));
        }
        
        // qInfo() << "Done saving media. Saved " << count << " items";
        settings.endArray();        
    }catch(const std::exception &exec){
        qInfo() << "Error saving media.";
        qWarning() << "Error is: " << exec.what();
        qErrnoWarning("Error saving media. Check error logs for more information");
    }
}

void Project::loadMedia() {
    try{
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
    }catch(const std::exception &exec){
        qInfo() << "Error loading media.";
        qFatal() << "Error is: " << exec.what();
        qErrnoWarning("Error loading media. Check error logs for more information");
    }
}
