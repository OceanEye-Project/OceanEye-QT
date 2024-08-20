#include "detectoptions.h"
#include "./ui_detectoptions.h"

#include <QPushButton>

DetectOptions::DetectOptions(std::shared_ptr<Project>& project)
    : QWidget{}
    , ui(new Ui::DetectOptions)
    , currentProject(project)
{
    ui->setupUi(this);
    setWindowTitle("Detection Options");

    // Populate the drop down menu
    ui->classSelect->addItem("All");
    for (int i=0; i<model_classes.size();i++) {
        ui->classSelect->addItem(
            QString::fromStdString(model_classes.at(i)),
            QVariant(i)
            );
    }

    connect(ui->cancelBtn, &QPushButton::clicked, this, &DetectOptions::close);
    connect(ui->okBtn, &QPushButton::clicked, this, [this]() {
        if (ui->classSelect->currentText() == "All")
            emit runDetection();
        else
            emit runSpecificDetection(ui->classSelect->currentText());

        DetectOptions::close();
    });
}
