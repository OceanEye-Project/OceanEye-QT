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

#include "../../util/project.h"
#include "../../util/settings.h"

class ProjectSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectSettings(std::shared_ptr<Project>& project);
    std::shared_ptr<Project>& currentProject;
signals:
    void updateImageUI();
};

#endif // PROJECTSETTINGS_H
