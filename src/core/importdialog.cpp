
#include "importdialog.h"
#include "./ui_importdialog.h"
#include <fstream>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>

ImportWaitingDialog::ImportWaitingDialog() {
    setWindowTitle("Importing...");
    setFixedSize(300, 100);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setLayout(new QVBoxLayout());

    progressBar = new QProgressBar();
    progressBar->setRange(0, 0);

    progressLabel = new QLabel("Importing...");

    layout()->addWidget(progressLabel);
    layout()->addWidget(progressBar);

    connect(this, &ImportWaitingDialog::setLabelText, this, [this](QString text){
        progressLabel->setText(text);
    });
}

ImportDialog::ImportDialog(std::shared_ptr<Project>& project, QWidget *parent)
    : QDialog{parent}
    , ui(new Ui::ImportDialog)
    , currentProject(project)
    , dialog()
{
    ui->setupUi(this);

    connect(ui->okBtn, &QPushButton::clicked, this, &ImportDialog::doImport);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &ImportDialog::close);

    connect(&watcher, &QFutureWatcher<void>::finished, this, [this]{
        qInfo() << "Import Finished";
        dialog.hide();
        emit doneImport();
    });

}

/** Import COCO Window
 * 
 * @brief: When the user clicks the import button, this function is called.
 * It checks the selected format and calls the appropriate import function.
 * 
 * Options: All Data
 * @param: None
 */
void importCOCO(std::shared_ptr<Project>& currentProject, QString saveLocation, ImportWaitingDialog& dialog) {
    if (saveLocation.isEmpty()) {
        return; // User canceled the save dialog
    }

    std::filesystem::path rootDirectory = {
        std::filesystem::path(saveLocation.toStdString()).parent_path()
    };


    QFile inFile(saveLocation);
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);

    if (doc.isNull()) {
        qDebug() << "Parse failed";
    }

    QJsonObject rootObj = doc.object();
    QJsonArray categoryArray = rootObj.value("categories").toArray();
    QJsonArray imgArray = rootObj.value("images").toArray();
    QJsonArray annotationArray = rootObj.value("annotations").toArray();

    std::map<int, QString> imageIds {};
    std::map<int, QString> classMap {};
    std::map<QString, std::vector<Annotation>> importedAnnotations {};

    int loaded = 0;

    foreach(const QJsonValue& category, categoryArray){
        int id = category.toObject().value("id").toInt();
        QString name = category.toObject().value("name").toString();

        classMap.insert({id, name});

        emit dialog.setLabelText(QString("Loading %1 classes").arg(++loaded));
    }

    loaded = 0;

    foreach(const QJsonValue& image, imgArray){
        int id = image.toObject().value("id").toInt();
        QString fileName = image.toObject().value("file_name").toString();
        imageIds.insert({id, fileName});
        importedAnnotations.insert({fileName, {}});

        emit dialog.setLabelText(QString("Loading %1 images").arg(++loaded));
    }
    

    foreach(const QJsonValue& annotation, annotationArray){
        int imgId = annotation.toObject().value("image_id").toInt();
        QString fileName = imageIds.at(imgId);

        int classId = annotation.toObject().value("category_id").toInt();

        QJsonArray jsonBbox = annotation.toObject().value("bbox").toArray();

        QPoint boxCoords {
            jsonBbox.at(0).toDouble(),
            jsonBbox.at(1).toDouble()
        };

        QSize boxSize {
            jsonBbox.at(2).toDouble(),
            jsonBbox.at(3).toDouble()
        };

        QRect box {boxCoords, boxSize};

        Annotation image_annotation {
            classId,
            classMap.at(classId),
            1.0,
            box
        };

        importedAnnotations.at(fileName).push_back(image_annotation);

        emit dialog.setLabelText(QString("Loading %1 annotations").arg(++loaded));
    }
    
    loaded = 0;
    for (auto const& [filename, annotations] : importedAnnotations) {
        QString filepath = QString::fromStdString(
            (rootDirectory / filename.toStdString()).string()
        );
        currentProject->media.push_back(filepath);
        currentProject->setAnnotation(filepath, annotations);
        currentProject->saveMedia();

        emit dialog.setLabelText(QString("Saving %1 annotations").arg(++loaded));
    }
}

/** Import Window
 * 
 * @brief: When the user clicks the import button, this function is called.
 * It checks the selected format and calls the appropriate import function.
 * 
 * Options: All CSV
 * @param: None
 */
void ImportDialog::doImport() {
    QString saveLocation = QFileDialog::getOpenFileName(nullptr, "Open COCO", currentProject->projectPath, "COCO (*.json)");

    dialog.show();

    std::function<void()> importWithProject = [this, saveLocation]() {
        if (ui->formatCombo->currentText() == "COCO") {
            importCOCO(currentProject, saveLocation, dialog);
        }
    };

    future = QtConcurrent::run(importWithProject);
    watcher.setFuture(future);
}



