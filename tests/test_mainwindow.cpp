// Executable command: ./build/OceanEyeMainWindowTest.app/Contents/MacOS/OceanEyeMainWindowTest
#include <QtTest/QtTest>
#include "../src/core/mainwindow.h"
#include "../src/core/welcomewindow.h"

class MainWindowTest : public QObject
{
    Q_OBJECT

private:
    std::shared_ptr<Project> project;
    MainWindow* mainWindow;

private slots:
    void init();
    void cleanup();
    void testConstructor();
    void testLoadModel();
    void testFilterDeadVideo();
};

void MainWindowTest::init() {
    WelcomeWindow welcomeWindow(project);
    welcomeWindow.loadProjectFromPath("test_project");
    mainWindow = new MainWindow(project);
}

void MainWindowTest::cleanup() {
    delete mainWindow;
    mainWindow = nullptr;
}

void MainWindowTest::testConstructor() {
    QVERIFY(mainWindow->ui != nullptr);
    QVERIFY(mainWindow->currentProject == project);
}

void MainWindowTest::testLoadModel()
{
}

void MainWindowTest::testFilterDeadVideo() {
    mainWindow->loadModel("../models/large_model.onnx");

    mainWindow->addMedia({"OceanEyeVid.mov"});
    
    mainWindow->currentProject->settings.setValue("Automatically Filter Dead Video", true);
    
    QSignalSpy spy(mainWindow, &MainWindow::doneSlicing);
    spy.wait(8000);

    // Check the media size
    qDebug() << "MEDIA SIZE = " << mainWindow->currentProject->media.size();
    QVERIFY(mainWindow->currentProject->media.size() == 13);
}


void testLoadModel(){
    
}

// Register the test class
QTEST_MAIN(MainWindowTest)
#include "test_mainwindow.moc"
