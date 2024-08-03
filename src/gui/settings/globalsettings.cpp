#include "globalsettings.h"

GlobalSettings::GlobalSettings()
    : QWidget{}
{
    QFormLayout* layout = new QFormLayout();
    setLayout(layout);

    QSettings settings {"oceaneye", "oceaneye"};

    QLabel* settingsPathLabel = new QLabel(settings.fileName());
    settingsPathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    layout->addRow("Settings Path", settingsPathLabel);
}
