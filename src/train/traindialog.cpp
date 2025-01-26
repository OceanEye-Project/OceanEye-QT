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
}

void TrainDialog::startTraining() {
    TrainArgs trainArgs {
        .model = ui->baseModelLineEdit->text().simplified().toStdString(),
        .time = ui->timeDoubleSpinBox->value(),
        .epochs = ui->epochsSpinBox->value(),
        .patience = ui->patienceSpinBox->value()
    };

    modelTrainer.startTraining(trainArgs);
}
