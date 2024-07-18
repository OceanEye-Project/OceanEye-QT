// Executable command: ./build/OceanEyeTest.app/Contents/MacOS/OceanEyeTest
#include <QtTest/QtTest>
#include "../welcomewindow.h"

class WelcomeWindowTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testConstructor();
    void testLoadProjectPaths();
    void testSaveProjectPaths();
    void testLoadProjectFromPath();
};

// Define the test functions
void WelcomeWindowTest::initTestCase() {
    // Initialization code here
}

void WelcomeWindowTest::cleanupTestCase() {
    // Cleanup code here
}

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
    settings.setValue("path0", "test_path");
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
    // Test code here
}

void WelcomeWindowTest::testLoadProjectFromPath() {
    // Test code here
}

// Register the test class
QTEST_MAIN(WelcomeWindowTest)
#include "test_welcomewindow.moc"
