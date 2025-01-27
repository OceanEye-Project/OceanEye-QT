#ifndef TRAINDIALOG_H
#define TRAINDIALOG_H

#include <QDialog>
#include <QFileDialog>

#include "project.h"
#include "modeltrainer.h"

namespace Ui {
class TrainDialog;
}

class TrainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TrainDialog(std::shared_ptr<Project>& project);

private:
    Ui::TrainDialog *ui;

    ModelTrainer modelTrainer;

    std::shared_ptr<Project>& currentProject;

    void startTraining();
};

#endif // TRAINDIALOG_H
