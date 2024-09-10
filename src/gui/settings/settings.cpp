#include "settings.h"

Settings::Settings(std::shared_ptr<Project>& currentProject)
    : QWidget{}
    , globalSettings{}
    , projectSettings{currentProject}
{
    QHBoxLayout* layout = new QHBoxLayout();
    setLayout(layout);

    layout->addWidget(&projectSettings);
}