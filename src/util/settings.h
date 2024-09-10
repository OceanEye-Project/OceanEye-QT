#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>
#include <QVariant>
#include <QString>
#include <QSettings>
#include <QIODevice>
#include <QDataStream>
#include <QMetaType>
#include <QKeySequence>
#include <yaml-cpp/yaml.h>
#include <iostream>

typedef QSettings::SettingsMap::const_iterator SettingsIterator;

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
    {"Automatically Filter Dead Video", false}
};

const std::vector<Setting> defaultGlobalSettings {};

QSettings::Format registerYAMLFormat();

#endif // SETTINGS_H
