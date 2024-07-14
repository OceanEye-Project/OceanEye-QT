
#include "exportdialog.h"
#include "./ui_exportdialog.h"

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
        annotations.at(image) = currentProject->getAnnotation(image);
        totalAnnotations += annotations.at(image).size();
    }

    ui->exportCount->setText(QString("Exporting %1 annotations").arg(totalAnnotations));


}

void exportCSV(QString filename, std::map<QString, std::vector<Annotation>>& annotations) {
// TODO
}


void exportJSON(QString filename, std::map<QString, std::vector<Annotation>>& annotations) {
// TODO
}

void ExportDialog::doExport() {
    if (ui->formatCombo->currentText() == "CSV") {
        exportCSV("all_annotations.csv", annotations);
    } else if (ui->formatCombo->currentText() == "JSON") {
        exportJSON("all_annotations.json", annotations);
    }
}



