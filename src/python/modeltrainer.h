#ifndef MODELTRAINER_H
#define MODELTRAINER_H

#include <QObject>
#include "project.h"

class ModelTrainer : public QObject
{
    Q_OBJECT

    std::shared_ptr<Project>& currentProject;

public:
    explicit ModelTrainer(std::shared_ptr<Project>& project);

    void startTraining();
};

#endif // MODELTRAINER_H
