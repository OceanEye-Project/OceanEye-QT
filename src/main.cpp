#include "core/welcomewindow.h"
#include "core/mainwindow.h"
#include <QApplication>
#include "util/project.h"
#include "logger.h"

std::shared_ptr<Project> project = nullptr;

int main(int argc, char *argv[])
{   
    // Initialize Logger
    Logger::Logger::init("log.txt");

    QApplication app(argc, argv);
    WelcomeWindow window(project);
    window.setWindowTitle("OceanEye");

    // app.setStyle("Fusion");
    // QPalette palette = app.palette();
    // palette.setColor(QPalette::ColorRole::Link, QColor(82, 166, 227));

    // app.setPalette(palette);

    // can be changed to light theme just by replacing "dark" with "light"
    QFile file(":qdarkstyle/dark/darkstyle.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());

    app.setStyleSheet(styleSheet);

    // Execute app and capture return
    window.show();
    int result = app.exec();

    // Cleanup after logger.
    Logger::Logger::cleanup();

    // Return exit status
    return result;
}
