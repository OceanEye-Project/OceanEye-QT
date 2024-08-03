#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTabWidget>
#include "../../util/project.h"
#include "globalsettings.h"
#include "projectsettings.h"

class Settings : public QWidget
{
    Q_OBJECT
public:
    explicit Settings(std::shared_ptr<Project>& currentProject);
    GlobalSettings globalSettings;
    ProjectSettings projectSettings;

signals:
};

#endif // SETTINGS_H
