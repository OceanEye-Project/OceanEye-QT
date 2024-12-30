#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "mainwindow.h"
#include "../util/project.h"
#include "../gui/settings/globalsettings.h"
#include "../python/python.h"

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

    Ui::WelcomeWindow* getUI() const { return ui; }
    std::shared_ptr<Project> getCurrentProject() const { return currentProject; }
    std::vector<QString> getProjects() const { return projects; }
    friend class WelcomeWindowTest;
    friend class MainWindowTest;
    friend class MainWindow;

private:
    std::shared_ptr<Project>& currentProject;
    std::vector<QString> projects {};

    Ui::WelcomeWindow *ui;

    void loadProjectPaths();
    void loadProjectFromPath(QString projectPath, bool isNewProject = false);
    void saveProjectPaths();

public slots:
    void openProject(bool isNewProject = false);

signals:
    void projectOpened();
};
#endif // WELCOMEWINDOW_H
