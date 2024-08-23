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
    void emptyTestProject();    

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

    // Remove all files inside of test_project
    QDir testDir("test_project");
    if (!testDir.exists()) {
        qWarning() << "Directory does not exist.";
        return;
    }

    // List all files in the directory
    QStringList files = testDir.entryList(QDir::Files);

    // Iterate through the list and remove each file
    for (const QString &fileName : files) {
        if (testDir.remove(fileName)) {
            qDebug() << "Removed file:" << fileName;
        } else {
            qWarning() << "Failed to remove file:" << fileName;
        }
    }

    if (testDir.remove(".oceaneye.ini")) {
        qDebug() << "Removed file:" << ".oceaneye.ini";
    } else {
        qWarning() << "Failed to remove file:" << ".oceaneye.ini";
    }
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
    emptyTestProject();
    mainWindow->loadModel("../models/large_model.onnx");

    mainWindow->addMedia({"OceanEyeVid.mov"});
    
    mainWindow->currentProject->settings.setValue("Automatically Filter Dead Video", true);
    
    QSignalSpy spy(mainWindow, &MainWindow::doneSlicing);
    spy.wait(8000);

    // Check the media size
    qDebug() << "MEDIA SIZE = " << mainWindow->currentProject->media.size();
    QVERIFY(mainWindow->currentProject->media.size() == 13);
}

void MainWindowTest::emptyTestProject() {
}

void testLoadModel(){
    
}

// Register the test class
QTEST_MAIN(MainWindowTest)
#include "test_mainwindow.moc"
