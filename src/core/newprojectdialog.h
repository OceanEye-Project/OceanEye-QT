#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include "../util/project.h"
#include "../util/yolov8.h"

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    std::shared_ptr<Project>& currentProject;
    explicit NewProjectDialog(std::shared_ptr<Project>& project);
    void createProject();
    void show(QString path, std::function<void()> callback);

private:
    Ui::NewProjectDialog *ui;
};

#endif // NEWPROJECTDIALOG_H
