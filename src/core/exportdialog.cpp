
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
    }
}



