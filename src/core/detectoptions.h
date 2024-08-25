#ifndef DETECTOPTIONS_H
#define DETECTOPTIONS_H

#include "../util/project.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class DetectOptions;
}
QT_END_NAMESPACE

class DetectOptions : public QWidget
{
    Q_OBJECT
    Ui::DetectOptions *ui;
    std::shared_ptr<Project>& currentProject;
public:
    explicit DetectOptions(std::shared_ptr<Project>& project);

signals:
    void runDetection();
    void runSpecificDetection(QList<QListWidgetItem *> classType);
};

#endif // DETECTOPTIONS_H
