#include "projectsettings.h"
#include <QPushButton>

ProjectSettings::ProjectSettings(std::shared_ptr<Project>& project)
    : QWidget{}
    , currentProject{project}
{
    QFormLayout* layout = new QFormLayout();
    setLayout(layout);

    QSettings& settings {currentProject->settings};
    QLabel* settingsPathLabel = new QLabel(settings.fileName());
    settingsPathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QPushButton* applyButton = new QPushButton("Apply", this);  // Create the button with "Apply" text
    layout->addWidget(applyButton);  // Add the button to the layout

    layout->addRow("Settings Path", settingsPathLabel);

    // Map to store the current values of the widgets
    std::map<QString, QWidget*> widgetMap;

    // Create widgets for settings
    for (auto& setting : defaultProjectSettings) {
        if (!settings.contains(setting.key))
            settings.setValue(setting.key, setting.defaultValue);

        switch (setting.defaultValue.typeId()) {
            case QMetaType::Int: {
                QSpinBox* box = new QSpinBox();
                box->setSuffix(setting.suffix);
                box->setRange(setting.minValue, setting.maxValue);
                box->setValue(settings.value(setting.key).toInt());
                widgetMap[setting.key] = box;  // Store the widget
                layout->addRow(setting.key, box);
                break;
            }
            case QMetaType::Double: {
                QDoubleSpinBox* box = new QDoubleSpinBox();
                box->setSuffix(setting.suffix);
                box->setRange(setting.minValue, setting.maxValue);
                box->setValue(settings.value(setting.key).toDouble());
                widgetMap[setting.key] = box;  // Store the widget
                layout->addRow(setting.key, box);
                break;
            }
            case QMetaType::Bool: {
                QCheckBox* box = new QCheckBox(setting.suffix);
                box->setChecked(settings.value(setting.key).toBool());
                widgetMap[setting.key] = box;  // Store the widget
                layout->addRow(setting.key, box);
                break;
            }
            case QMetaType::QString: {
                QLineEdit* box = new QLineEdit();
                box->setText(settings.value(setting.key).toString());
                widgetMap[setting.key] = box;  // Store the widget
                layout->addRow(setting.key, box);
                break;
            }
            default:
                break;
        }
    }

    // Apply button clicked signal
    connect(applyButton, &QPushButton::clicked, this, [this, &settings, widgetMap]() {
        for (auto& setting : defaultProjectSettings) {
            QWidget* widget = widgetMap.at(setting.key);
            QVariant newValue;

            switch (setting.defaultValue.typeId()) {
                case QMetaType::Int:
                    newValue = dynamic_cast<QSpinBox*>(widget)->value();
                    currentProject->setModelConf(newValue.toInt());
                    break;
                case QMetaType::Double:
                    newValue = dynamic_cast<QDoubleSpinBox*>(widget)->value();
                    break;
                case QMetaType::Bool:
                    newValue = dynamic_cast<QCheckBox*>(widget)->isChecked();
                    break;
                case QMetaType::QString:
                    newValue = dynamic_cast<QLineEdit*>(widget)->text();
                    break;
                default:
                    break;
            }

            // Only update if the value has changed
            if (settings.value(setting.key) != newValue) {
                settings.setValue(setting.key, newValue);
            }
        }

        if (!currentProject->media.empty())
            for(auto image : currentProject->media) {
                currentProject->runDetection(image);
            }
        updateImageUI();
    });
}
