// Executable command: ./build/OceanEyeMainWindowTest.app/Contents/MacOS/OceanEyeMainWindowTest
#include <QtTest/QtTest>
#include "../mainwindow.h"

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
    void testUpdateTable();
    void testLoadModel();
};

void MainWindowTest::init(){
    project = nullptr;
    mainWindow = new MainWindow(project);
}

void MainWindowTest::cleanup(){
    delete mainWindow;
    mainWindow = nullptr;
}

void MainWindowTest::testConstructor() {
    QVERIFY(mainWindow->ui != nullptr);
    QVERIFY(mainWindow->currentProject == project);
}

void MainWindowTest::testUpdateTable() {
    mainWindow->updateTable();
    QVERIFY(mainWindow->model->horizontalHeaderItem(0)->text() == "Class");
    QVERIFY(mainWindow->model->horizontalHeaderItem(1)->text() == "Confidence");
    QVERIFY(mainWindow->model->horizontalHeaderItem(2)->text() == "X");
    QVERIFY(mainWindow->model->horizontalHeaderItem(3)->text() == "Y");
    QVERIFY(mainWindow->model->horizontalHeaderItem(4)->text() == "Width");
    QVERIFY(mainWindow->model->horizontalHeaderItem(5)->text() == "Height");
}

void MainWindowTest::testLoadModel()
{
    
}

void testLoadModel(){
    
}

// Register the test class
QTEST_MAIN(MainWindowTest)
#include "test_mainwindow.moc"
