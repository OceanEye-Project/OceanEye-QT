#include "welcomewindow.h"
#include "mainwindow.h"
#include <QApplication>
#include "project.h"

std::shared_ptr<Project> project = nullptr;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WelcomeWindow w(project);
    w.setWindowTitle("OceanEye");
    w.show();
    return a.exec();
}
