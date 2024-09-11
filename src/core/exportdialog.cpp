
#include "exportdialog.h"
#include "./ui_exportdialog.h"
#include <fstream>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>

ExportDialog::ExportDialog(std::shared_ptr<Project>& project, QWidget *parent)
    : QDialog{parent}
    , ui(new Ui::ExportDialog)
    , currentProject(project)
{
    ui->setupUi(this);

    connect(ui->okBtn, &QPushButton::clicked, this, &ExportDialog::doExport);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &ExportDialog::close);

}

/**
 * Export CSV
 * 
 * @brief: This function exports all annotations to a CSV file and saves it to the same directory.
 * 
 * 
 * 
 */

void exportCSV(QString filename, std::map<QString, std::vector<Annotation>>& annotations) {
    QString saveLocation = QFileDialog::getSaveFileName(nullptr, "Save CSV", filename, "CSV Files (*.csv)");
    if (saveLocation.isEmpty()) {
        return; // User canceled the save dialog
    }

    // Open file for writing, write header row
    std::ofstream outFile (saveLocation.toStdString());
    outFile << "Frame Name, Class, Confidence, X, Y, Width, Height\n";

    // Loop through annotations and write each row
    for (auto& [image, anns] : annotations) {
        for (auto& ann : anns) {
            outFile << image.toStdString() << ", "  << ann.className.toStdString() << ", " << ann.confidence << ", " 
            << ann.box.x() << ", " << ann.box.y() << ", " << ann.box.width() << ", " << ann.box.height() << "\n";
        }
    }
    // Close file
    outFile.close();
}

/** Export JSON Window
 * 
 * @brief: When the user clicks the export button, this function is called.
 * It checks the selected format and calls the appropriate export function.
 * 
 * Options: All JSON
 * @param: None
 */
void exportJSON(QString filename, std::map<QString, std::vector<Annotation>>& annotations) {
    QString saveLocation = QFileDialog::getSaveFileName(nullptr, "Save JSON", filename, "JSON Files (*.json)");
    if (saveLocation.isEmpty()) {
        return; // User canceled the save dialog
    }

    QJsonArray jsonAnnotations;

    // Loop through annotations and create JSON objects
    for (auto& [image, anns] : annotations) {
        for (auto& ann : anns) {
            QJsonObject jsonAnn;
            jsonAnn["frame_name"] = image;
            jsonAnn["class"] = ann.className;
            jsonAnn["confidence"] = ann.confidence;
            jsonAnn["x"] = ann.box.x();
            jsonAnn["y"] = ann.box.y();
            jsonAnn["width"] = ann.box.width();
            jsonAnn["height"] = ann.box.height();
            jsonAnnotations.append(jsonAnn);
        }
    }

    QJsonDocument jsonDoc(jsonAnnotations);
    QFile jsonFile(saveLocation);
    if (!jsonFile.open(QIODevice::WriteOnly)) {
        return; // Failed to open file
    }

    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();
}

/** Export COCO Window
 * 
 * @brief: When the user clicks the export button, this function is called.
 * It checks the selected format and calls the appropriate export function.
 * 
 * Options: All Data
 * @param: None
 */
void exportCOCO(QString filename, std::map<QString, std::vector<Annotation>>& annotations) {
    QString saveLocation = QFileDialog::getSaveFileName(nullptr, "Save COCO", filename, "COCO Files (*.json)");
    if (saveLocation.isEmpty()) {
        return; // User canceled the save dialog
    }

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
}

/** Export YAML Window
 * 
 * @brief: When the user clicks the export button, this function is called.
 * It checks the selected format and calls the appropriate export function.
 * 
 * Options: All Data
 * @param: None
 */
void exportYAML(QString filename, std::map<QString, std::vector<Annotation>>& annotations) {
    QString saveLocation = QFileDialog::getSaveFileName(nullptr, "Save COCO", filename, "YAML Files (*.yaml)");
    if (saveLocation.isEmpty()) {
        return; // User canceled the save dialog
    }

    YAML::Emitter out;
    out << YAML::BeginSeq;

    // Loop through annotations and create YAML objects
    for (auto& [image, anns] : annotations) {
        for (auto& ann : anns) {
            out << YAML::BeginMap;

            out << YAML::Key << "frame_name" << YAML::Value << image.toStdString();
            out << YAML::Key << "class" << YAML::Value << ann.className.toStdString();
            out << YAML::Key << "confidence" << YAML::Value << ann.confidence;
            out << YAML::Key << "x" << YAML::Value << ann.box.x();
            out << YAML::Key << "y" << YAML::Value << ann.box.y();
            out << YAML::Key << "width" << YAML::Value << ann.box.width();
            out << YAML::Key << "height" << YAML::Value << ann.box.height();

            out << YAML::EndMap;
        }
    }

    out << YAML::EndSeq;

    QFile yamlFile(saveLocation);
    if (!yamlFile.open(QIODevice::WriteOnly)) {
        return; // Failed to open file
    }

    yamlFile.write(out.c_str());
    yamlFile.close();
}

/** Export CSV Window
 * 
 * @brief: When the user clicks the export button, this function is called.
 * It checks the selected format and calls the appropriate export function.
 * 
 * Options: All CSV
 * @param: None
 */
void ExportDialog::doExport() {
    annotations = {};
    int totalAnnotations = 0;

    for (auto& image : currentProject->media) {
        annotations.insert({image, currentProject->getAnnotation(image)});
        totalAnnotations += annotations.at(image).size();
    }

    ui->exportCount->setText(QString("Exporting %1 annotations").arg(totalAnnotations));

    if (ui->formatCombo->currentText() == "CSV") {
        exportCSV("all_annotations.csv", annotations);
    } else if (ui->formatCombo->currentText() == "JSON") {
        exportJSON("all_annotations.json", annotations);
    } else if (ui->formatCombo->currentText() == "COCO") {
        exportCOCO("all_annotations.json", annotations);
    } else if (ui->formatCombo->currentText() == "YAML") {
        exportYAML("all_annotations.yaml", annotations);
    }
}



