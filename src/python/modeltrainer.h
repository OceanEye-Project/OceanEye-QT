#ifndef MODELTRAINER_H
#define MODELTRAINER_H

#include <QObject>
#include <QCoreApplication>
#include <iostream>
#include <filesystem>

#include "project.h"

// python and QT both use the macro "slots"
#pragma push_macro("slots")
#undef slots
#include <Python.h>
#include <pybind11/embed.h>
#pragma pop_macro("slots")

namespace py = pybind11;

class ModelTrainer : public QObject
{
    Q_OBJECT

    std::shared_ptr<Project>& currentProject;

public:
    explicit ModelTrainer(std::shared_ptr<Project>& project);

    void startTraining();
};

#endif // MODELTRAINER_H
