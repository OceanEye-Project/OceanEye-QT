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

    friend class WelcomeWindowTest;

private:
    std::shared_ptr<Project>& currentProject;
    std::vector<QString> projects {};

    Ui::WelcomeWindow *ui;

    void loadProjectPaths();
    void loadProjectFromPath(QString projectPath);
    void saveProjectPaths();

public slots:
    void openProject();

signals:
    void projectOpened();
};
#endif // WELCOMEWINDOW_H
