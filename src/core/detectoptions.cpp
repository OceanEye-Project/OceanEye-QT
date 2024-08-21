#include "detectoptions.h"
#include "./ui_detectoptions.h"

DetectOptions::DetectOptions(std::shared_ptr<Project>& project)
    : QWidget{}
    , ui(new Ui::DetectOptions)
    , currentProject(project)
{
    ui->setupUi(this);
    setWindowTitle("Detection Options");
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    ui->classList->setSelectionMode(QAbstractItemView::MultiSelection);

    // Populate the drop down menu
    ui->classList->addItem("All");
    for (int i=0; i<model_classes.size();i++) {
        ui->classList->addItem(
            QString::fromStdString(model_classes.at(i))
        );
    }

    connect(ui->cancelBtn, &QPushButton::clicked, this, &DetectOptions::close);
    connect(ui->okBtn, &QPushButton::clicked, this, [this]() {
        if (ui->classList->findItems("All", Qt::MatchExactly).first()->isSelected())
            emit runDetection();
        else if (ui->classList->selectedItems().size() > 0)
            emit runSpecificDetection(ui->classList->selectedItems());

        DetectOptions::close();
    });
}
