#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "mainwindow.h"
#include "project.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class WelcomeWindow;
}
QT_END_NAMESPACE

class WelcomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    WelcomeWindow(std::shared_ptr<Project>& currentProject, QWidget *parent = nullptr);
    ~WelcomeWindow();

private:
    std::shared_ptr<Project>& currentProject;
    std::vector<QString> projects {};

    Ui::WelcomeWindow *ui;

    void loadProjectPaths();
    void saveProjectPaths();

    void loadProjectFromPath(QString projectPath);

public slots:
    void openProject();
};
#endif // WELCOMEWINDOW_H
