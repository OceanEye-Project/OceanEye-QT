#include "traindialog.h"
#include "ui_traindialog.h"

TrainDialog::TrainDialog(std::shared_ptr<Project>& project)
    : QDialog()
    , ui(new Ui::TrainDialog)
    , currentProject(project)
    , modelTrainer(project)
{
    ui->setupUi(this);

    connect(ui->trainBtn, &QPushButton::clicked, this, &TrainDialog::startTraining);

    connect(ui->saveLocationBrowseBtn,  &QPushButton::clicked, this, [this](){
        QString fileLocation = QFileDialog::getSaveFileName(
            this,
            "Trained Model Output",
            currentProject->projectPath,
            "Models (*.onnx)"
        );
        ui->saveLocation->setText(fileLocation);
    });
}

void TrainDialog::startTraining() {
    QFileInfo saveLocation { ui->saveLocation->text() };

    if (saveLocation.suffix() != "onnx" || saveLocation.exists()) {
        ui->saveLocation->setStyleSheet("border: 1px solid red");
        return;
    } else {
        ui->saveLocation->setStyleSheet("");
    }


    TrainArgs trainArgs {
        .model = ui->baseModelLineEdit->text().simplified().toStdString(),
        .saveFilePath = saveLocation.absoluteFilePath(),
        .time = ui->timeDoubleSpinBox->value(),
        .epochs = ui->epochsSpinBox->value(),
        .patience = ui->patienceSpinBox->value()
    };

    modelTrainer.startTraining(trainArgs);
}
