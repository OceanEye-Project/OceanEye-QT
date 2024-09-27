#include "projectsettings.h"
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

ProjectSettings::ProjectSettings(std::shared_ptr<Project>& project)
    : QWidget{}
    , currentProject{project}
    , detectOptionsDialog{project}
{
    // Main layout for the entire window
    QVBoxLayout* mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    // GroupBox for the "Apply" button section with a border
    QGroupBox* applyGroupBox = new QGroupBox("Apply Section");
    QHBoxLayout* applyLayout = new QHBoxLayout();
    applyGroupBox->setLayout(applyLayout);
    mainLayout->addWidget(applyGroupBox);

    // "Apply" button placed in the group box
    QPushButton* applyButton = new QPushButton("Apply", this);
    applyLayout->addWidget(applyButton, 0, Qt::AlignLeft);  // Align the button to the left

    // GroupBox for the settings section with a border
    QGroupBox* settingsGroupBox = new QGroupBox("Settings Section");
    QFormLayout* settingsLayout = new QFormLayout();
    settingsGroupBox->setLayout(settingsLayout);
    mainLayout->addWidget(settingsGroupBox);

    QPushButton* detectOptionsButton = new QPushButton("Specify Detections", this);
    settingsLayout->addRow(detectOptionsButton);
    connect(detectOptionsButton, &QPushButton::clicked, &detectOptionsDialog, &DetectOptions::show);

    QSettings& settings {currentProject->settings};
    QLabel* settingsPathLabel = new QLabel(settings.fileName());
    settingsPathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // Load Model Button and File Name layout as a form row
    QPushButton* loadModelButton = new QPushButton("Load Model", this);
    QLabel* modelFileLabel = new QLabel("");  // Label to display the selected file name
    modelFileLabel->setStyleSheet("color: grey;");  // Optional: Style the label to make it stand out

    // Add the button and label as a form row (consistent with the rest of the settings)
    settingsLayout->addRow(loadModelButton, modelFileLabel);
    
    // Map to store the current values of the widgets
    std::map<QString, QWidget*> widgetMap;

    // Create widgets for settings
    for (auto& setting : defaultProjectSettings) {
        if (!settings.contains(setting.key))
            settings.setValue(setting.key, setting.defaultValue);
        if(setting.key.compare("Model Path") == 0) continue; // Skip over Model Path setting because we've already have a button for it

        switch (setting.defaultValue.typeId()) {
            case QMetaType::Int: {
                QSpinBox* box = new QSpinBox();
                box->setSuffix(setting.suffix);
                box->setRange(setting.minValue, setting.maxValue);
                box->setValue(settings.value(setting.key).toInt());
                widgetMap[setting.key] = box;  // Store the widget
                settingsLayout->addRow(setting.key, box);
                // Update model conf slider
                break;
            }
            case QMetaType::Double: {
                QDoubleSpinBox* box = new QDoubleSpinBox();
                box->setSuffix(setting.suffix);
                box->setRange(setting.minValue, setting.maxValue);
                box->setValue(settings.value(setting.key).toDouble());
                widgetMap[setting.key] = box;  // Store the widget
                settingsLayout->addRow(setting.key, box);
                break;
            }
            case QMetaType::Bool: {
                QCheckBox* box = new QCheckBox(setting.suffix);
                box->setChecked(settings.value(setting.key).toBool());
                widgetMap[setting.key] = box;  // Store the widget
                settingsLayout->addRow(setting.key, box);
                break;
            }
             case QMetaType::QString: {
                QLineEdit* box = new QLineEdit();
                box->setText(settings.value(setting.key).toString());
                widgetMap[setting.key] = box;  // Store the widget
                settingsLayout->addRow(setting.key, box);
                break;
            }
            default:
                break;
        }
    }

    // Connect Load Model button to loadModel function
    connect(loadModelButton, &QPushButton::clicked, this, [this, modelFileLabel]() {
        QString file = QFileDialog::getOpenFileName(this, "Select one or more files to open", "", "Models (*.onnx)");
        if (!file.isEmpty()) {
            modelFileLabel->setText(file);  // Set the file name next to the button
            modelFileLabel->setToolTip(file);  // Optionally, show full path on hover as a tooltip
            currentProject->loadModel(file);  // Load the selected model
        }
    });

    // Apply button clicked signal
    connect(applyButton, &QPushButton::clicked, this, [this, &settings, widgetMap, mainLayout]() {
        for (auto& setting : defaultProjectSettings) {
            if(setting.key.compare("Model Path") == 0) continue; // Skip over Model Path setting because it has a separate process
            QWidget* widget = widgetMap.at(setting.key);
            QVariant newValue;

            switch (setting.defaultValue.typeId()) {
                case QMetaType::Int:
                    newValue = dynamic_cast<QSpinBox*>(widget)->value();
                    currentProject->setModelConf(newValue.toInt());
                    updateModelConfSlider();
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
        parentWidget()->close();
    });
}

void ProjectSettings::loadModel(QString file) {
    if (file == "") 
        file = QFileDialog::getOpenFileName(this, "Select one or more files to open", "", "Models (*.onnx)");
    currentProject->loadModel(file);
}
