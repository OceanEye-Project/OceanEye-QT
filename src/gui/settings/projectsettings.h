#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include <QWidget>
#include <QFormLayout>
#include <QSettings>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QDesktopServices>

#include "../../util/project.h"
#include "../../util/settings.h"
#include "../../core/detectoptions.h"

class ProjectSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectSettings(std::shared_ptr<Project>& project);
    std::shared_ptr<Project>& currentProject;
    void loadModel(QString file);
    DetectOptions detectOptionsDialog;
signals:
    void updateModelConfSlider();
    void updateImageUI();
};

#endif // PROJECTSETTINGS_H
