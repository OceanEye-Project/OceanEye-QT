
#include "exportdialog.h"
#include "./ui_exportdialog.h"
#include <fstream>
#include <iostream>
#include <QFileDialog>
ExportDialog::ExportDialog(std::shared_ptr<Project>& project, QWidget *parent)
    : QDialog{parent}
    , ui(new Ui::ExportDialog)
    , currentProject(project)
{
    ui->setupUi(this);

    connect(this, &QDialog::accepted, this, &ExportDialog::doExport);

    annotations = {};
    int totalAnnotations = 0;

    for (auto& image : currentProject->media) {
        annotations.insert({image, currentProject->getAnnotation(image)});
        totalAnnotations += annotations.at(image).size();
    }

    ui->exportCount->setText(QString("Exporting %1 annotations").arg(totalAnnotations));

}

// TODO: Add dynamic export functionality

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

    // Debug: Output CSV to console
    std::ifstream inFile (saveLocation.toStdString());
    std::string line;
    while (std::getline(inFile, line)) {
        std::cout << line << std::endl;
    }

    // Download the file
}


void exportJSON(QString filename, std::map<QString, std::vector<Annotation>>& annotations) {
// TODO
}
/** Export Window
 * 
 * @brief: When the user clicks the export button, this function is called.
 * It checks the selected format and calls the appropriate export function.
 * 
 * Options: All CSV or ALL JSON
 * @param: None
 */
void ExportDialog::doExport() {
    if (ui->formatCombo->currentText() == "CSV") {
        exportCSV("all_annotations.csv", annotations);
    } else if (ui->formatCombo->currentText() == "JSON") {
        exportJSON("all_annotations.json", annotations);
    }
}



