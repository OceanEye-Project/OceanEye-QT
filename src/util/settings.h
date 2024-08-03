#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>
#include <QVariant>
#include <QString>

struct Setting {
    QString key;
    QVariant defaultValue;
    QString suffix = "";
    int minValue = 0;
    int maxValue = 100;
    bool readOnly = false;
};

const std::vector<Setting> defaultProjectSettings {
    {"Slice Interval", 5, .suffix = " seconds", .minValue = 1, .maxValue = 6000},
    {"Model Path", ""},
    {"Model Confidence", 70, .suffix = " %", .minValue = 1, .maxValue = 100},
};

const std::vector<Setting> defaultGlobalSettings {};


#endif // SETTINGS_H
