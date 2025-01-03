
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
void importCOCO() {
    QString saveLocation = QFileDialog::getSaveFileName(nullptr, "Save COCO", filename, "COCO Files (*.json)");
    if (saveLocation.isEmpty()) {
        return; // User canceled the save dialog
    }

    /*

    QJsonObject jsonData;
    QJsonArray jsonAnnotations;
    QJsonArray jsonImages;
    QJsonArray categories;

    int current_image = 0;
    int current_annotation = 0;

    // Loop through annotations and create JSON objects
    for (auto& [image, anns] : annotations) {

        cv::Mat img = cv::imread(image.toStdString());

        QJsonObject jsonImg;
        jsonImg["id"] = ++current_image;
        jsonImg["width"] = img.cols;
        jsonImg["height"] = img.rows;
        jsonImg["file_name"] = image;
        jsonImg["license"] = 0;
        jsonImg["date_captured"] = 0;
        jsonImages.append(jsonImg);

        for (auto& ann : anns) {
            QJsonObject jsonAnn;
            float area = ann.box.width() * ann.box.height();

            QJsonArray bbox;
            bbox.append(ann.box.x());
            bbox.append(ann.box.y());
            bbox.append(ann.box.width());
            bbox.append(ann.box.height());

            QJsonArray segmentation;

            jsonAnn["id"] = current_annotation++;
            jsonAnn["image_id"] = current_image;
            jsonAnn["category_id"] = ann.classId;
            jsonAnn["area"] =  ann.box.width() * ann.box.height();
            jsonAnn["bbox"] = bbox;
            jsonAnn["segmentation"] = segmentation;
            jsonAnn["iscrowd"] = 0;
            jsonAnnotations.append(jsonAnn);

            // Check if category already exists, and add if not
            bool categoryExists = false;
            for (const auto& cat : categories) {
                if (cat.toObject()["id"] == ann.classId) {
                    categoryExists = true;
                    break;
                }
            }

            if (!categoryExists) {
                QJsonObject category;
                category["id"] = ann.classId;
                category["name"] = ann.className;
                category["supercategory"] = "";
                categories.append(category);
            }
        }
    }

    QJsonObject info;
    info["year"] = QDate::currentDate().year();
    info["version"] = "";
    info["description"] = "";
    info["contributor"] = "";
    info["url"] = "";
    info["date_created"] = QDate::currentDate().toString("yyyy-MM-dd");

    QJsonArray licenses;
    QJsonObject license;
    license["id"] = 0;
    license["name"] = "";
    license["url"] = "";
    licenses.append(license);

    jsonData["categories"] = categories;
    jsonData["info"] = info;
    jsonData["licenses"] = licenses;
    jsonData["images"] = jsonImages;
    jsonData["annotations"] = jsonAnnotations;

    QJsonDocument jsonDoc(jsonData);
    QFile jsonFile(saveLocation);
    if (!jsonFile.open(QIODevice::WriteOnly)) {
        return; // Failed to open file
    }

    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();

    */
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
        importCOCO();
    }
}



