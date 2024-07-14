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

    a.setStyle("Fusion");

    QFile file("/Users/ethan/OceanEye/styles.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());

    a.setStyleSheet(styleSheet);


    w.show();
    return a.exec();
}
