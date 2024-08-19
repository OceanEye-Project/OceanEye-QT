#include "settings.h"

Settings::Settings(std::shared_ptr<Project>& currentProject)
    : QWidget{}
    , globalSettings{}
    , projectSettings{currentProject}
{
    QHBoxLayout* layout = new QHBoxLayout();
    setLayout(layout);

    QTabWidget* tabs = new QTabWidget();
    layout->addWidget(tabs);

    tabs->addTab(&globalSettings, "Application");
    tabs->addTab(&projectSettings, "Project");
}