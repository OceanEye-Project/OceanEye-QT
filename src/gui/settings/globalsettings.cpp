#include "globalsettings.h"

GlobalSettings::GlobalSettings()
    : QWidget{}
{
    QFormLayout* layout = new QFormLayout();
    setLayout(layout);

    QSettings settings {QSettings::Scope::UserScope};

    QLabel* settingsPathLabel = new QLabel(settings.fileName());
    settingsPathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    layout->addRow("Settings Path", settingsPathLabel);
}
