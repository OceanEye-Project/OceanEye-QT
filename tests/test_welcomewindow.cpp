// Executable command: ./build/OceanEyeWelcomeWindowTest.app/Contents/MacOS/OceanEyeWelcomeWindowTest
#include <QtTest/QtTest>
#include "../src/core/welcomewindow.h"

class WelcomeWindowTest : public QObject
{
    Q_OBJECT

private slots:
    void testConstructor();
    void testLoadProjectPaths();
    void testSaveProjectPaths();
    void testLoadProjectFromPath();
};

void WelcomeWindowTest::testConstructor() {
    std::shared_ptr<Project> project;
    WelcomeWindow welcomeWindow(project);

    QVERIFY(welcomeWindow.getUI() != nullptr);
    QVERIFY(welcomeWindow.getCurrentProject() == project);
}

void WelcomeWindowTest::testLoadProjectPaths() {
    std::shared_ptr<Project> project;
    WelcomeWindow welcomeWindow(project);

    // Clear project paths and save settings
    QSettings settings("oceaneye", "oceaneye");
    settings.beginGroup("projects");
    settings.remove("");
    settings.endGroup();

    welcomeWindow.loadProjectPaths();
    QCOMPARE(welcomeWindow.getProjects().size(), 0);

    // Add a project path and save settings
    settings.beginGroup("projects");
    settings.setValue("path", "test_path");
    settings.endGroup();

    welcomeWindow.loadProjectFromPath("test_path");
    welcomeWindow.loadProjectPaths();

    // Clear project paths and save settings
    QCOMPARE(welcomeWindow.getProjects().size(), 1);
    QCOMPARE(welcomeWindow.getProjects()[0], QString("test_path"));

    // Clear project paths and save settings again after verification
    settings.beginGroup("projects");
    settings.remove("");
    settings.endGroup();

    welcomeWindow.loadProjectPaths();
    QCOMPARE(welcomeWindow.getProjects().size(), 0);
}

void WelcomeWindowTest::testSaveProjectPaths() {
    std::shared_ptr<Project> project;
    WelcomeWindow welcomeWindow(project);

    welcomeWindow.projects = {"test_path1", "test_path2"};
    welcomeWindow.saveProjectPaths();

    QSettings settings("oceaneye", "oceaneye");
    settings.beginReadArray("projects");

    QCOMPARE(settings.value("path").toString(), QString("test_path1"));
    settings.setArrayIndex(1); // Move to the next item in the array
    QCOMPARE(settings.value("path").toString(), QString("test_path2"));

    settings.endArray();
}

void WelcomeWindowTest::testLoadProjectFromPath() {
    std::shared_ptr<Project> project;
    WelcomeWindow welcomeWindow(project);

    QSignalSpy spy(&welcomeWindow, &WelcomeWindow::projectOpened);

    welcomeWindow.loadProjectFromPath("test_project_path");

    QVERIFY(welcomeWindow.getCurrentProject() != nullptr);
    QCOMPARE(welcomeWindow.getCurrentProject()->projectPath, QString("test_project_path"));

    QVERIFY(spy.count() == 1); // Ensure the signal is emitted
}

// Register the test class
QTEST_MAIN(WelcomeWindowTest)
#include "test_welcomewindow.moc"
