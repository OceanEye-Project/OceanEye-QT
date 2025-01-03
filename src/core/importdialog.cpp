
#include "importdialog.h"
#include "./ui_importdialog.h"
#include <fstream>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>

ImportDialog::ImportDialog(std::shared_ptr<Project>& project, QWidget *parent)
    : QDialog{parent}
    , ui(new Ui::ImportDialog)
    , currentProject(project)
{
    ui->setupUi(this);

    connect(ui->okBtn, &QPushButton::clicked, this, &ImportDialog::doImport);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &ImportDialog::close);

}

/** Import COCO Window
 * 
 * @brief: When the user clicks the import button, this function is called.
 * It checks the selected format and calls the appropriate import function.
 * 
 * Options: All Data
 * @param: None
 */
void importCOCO(std::shared_ptr<Project>& currentProject) {
    QString saveLocation = QFileDialog::getOpenFileName(nullptr, "Open COCO", "COCO Files (*.json)");
    if (saveLocation.isEmpty()) {
        return; // User canceled the save dialog
    }

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

    foreach(const QJsonValue& category, categoryArray){
        int id = category.toObject().value("id").toInt();
        QString name = category.toObject().value("name").toString();

        classMap.insert({id, name});
    }

    foreach(const QJsonValue& image, imgArray){
        int id = image.toObject().value("id").toInt();
        QString fileName = image.toObject().value("file_name").toString();
        imageIds.insert({id, fileName});
        importedAnnotations.insert({fileName, {}});
    }

    foreach(const QJsonValue& annotation, annotationArray){
        int imgId = annotation.toObject().value("image_id").toInt();
        QString fileName = imageIds.at(imgId);

        int classId = annotation.toObject().value("category_id").toInt();

        QJsonArray jsonBbox = annotation.toObject().value("bbox").toArray();

        QPoint boxCoords {
            jsonBbox.at(0).toInt(),
            jsonBbox.at(1).toInt()
        };

        QSize boxSize {
            jsonBbox.at(2).toInt(),
            jsonBbox.at(3).toInt()
        };

        QRect box {boxCoords, boxSize};

        Annotation image_annotation {
            classId,
            classMap.at(classId),
            1.0,
            box
        };

        importedAnnotations.at(fileName).push_back(image_annotation);
    }

    for (auto const& [filename, annotations] : importedAnnotations) {
        currentProject->media.push_back(filename);
        currentProject->setAnnotation(filename, annotations);
        currentProject->saveMedia();
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
    if (ui->formatCombo->currentText() == "COCO") {
        importCOCO(currentProject);
    }

    // TODO emit signal to reload media
}



