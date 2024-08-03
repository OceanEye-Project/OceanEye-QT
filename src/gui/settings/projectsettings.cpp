#include "projectsettings.h"

ProjectSettings::ProjectSettings(std::shared_ptr<Project>& project)
    : QWidget{}
    , currentProject{project}
{
    QFormLayout* layout = new QFormLayout();
    setLayout(layout);

    QSettings& settings {currentProject->settings};
    QLabel* settingsPathLabel = new QLabel(settings.fileName());
    settingsPathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    layout->addRow("Settings Path", settingsPathLabel);

    for (auto& setting : defaultProjectSettings) {
        if (!settings.contains(setting.key))
            settings.setValue(setting.key, setting.defaultValue);

        switch (setting.defaultValue.typeId()) {
            case QMetaType::Int: {
                    QSpinBox* box = new QSpinBox();
                    box->setSuffix(setting.suffix);
                    box->setRange(setting.minValue, setting.maxValue);
                    box->setValue(settings.value(setting.key).toInt());
                    connect(box, &QSpinBox::valueChanged, this, [&settings, &setting](QVariant newValue){
                        settings.setValue(setting.key, newValue);
                    });
                    layout->addRow(setting.key, box);
                    break;
            }
            case QMetaType::Double: {
                QDoubleSpinBox* box = new QDoubleSpinBox();
                box->setSuffix(setting.suffix);
                box->setRange(setting.minValue, setting.maxValue);
                box->setValue(settings.value(setting.key).toDouble());
                connect(box, &QDoubleSpinBox::valueChanged, this, [&settings, &setting](QVariant newValue){
                    settings.setValue(setting.key, newValue);
                });
                layout->addRow(setting.key, box);
                break;
            }
            case QMetaType::Bool: {
                QCheckBox* box = new QCheckBox(setting.suffix);
                box->setChecked(settings.value(setting.key).toBool());
                connect(box, &QCheckBox::checkStateChanged, this, [&settings, &setting](QVariant newValue){
                    settings.setValue(setting.key, newValue);
                });
                layout->addRow(setting.key, box);
                break;
            }
            case QMetaType::QString: {
                QLineEdit* box = new QLineEdit();
                box->setText(settings.value(setting.key).toString());
                connect(box, &QLineEdit::textChanged, this, [&settings, &setting](QVariant newValue){
                    settings.setValue(setting.key, newValue);
                });
                layout->addRow(setting.key, box);
                break;
            }
            default:
                break;
        }
    }



}
