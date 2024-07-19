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

void MainWindowTest::testUpdateTable(){
    mainWindow->updateTable();
    QVERIFY(mainWindow->model->horizontalHeaderItem(0) == new QStandardItem("Class"));
    QVERIFY(mainWindow->model->horizontalHeaderItem(1) == new QStandardItem("Confidence"));
    QVERIFY(mainWindow->model->horizontalHeaderItem(2) == new QStandardItem("X"));
    QVERIFY(mainWindow->model->horizontalHeaderItem(3) == new QStandardItem("Y"));
    QVERIFY(mainWindow->model->horizontalHeaderItem(4) == new QStandardItem("Width"));
    QVERIFY(mainWindow->model->horizontalHeaderItem(5) == new QStandardItem("Height"));
}

void MainWindow::testLoadModel(){

}

// Register the test class
QTEST_MAIN(MainWindowTest)
#include "test_mainwindow.moc"
