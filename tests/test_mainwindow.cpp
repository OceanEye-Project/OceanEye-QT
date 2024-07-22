// Executable command: ./build/OceanEyeTest.app/Contents/MacOS/OceanEyeMainWindowTest
#include <QtTest/QtTest>
#include "../src/core/mainwindow.h"

class MainWindowTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void mainWindowTest();
};

// Define the test functions
void MainWindowTest::initTestCase() {
    // Initialization code here
}

void MainWindowTest::mainWindowTest(){
    // code here
}

// Register the test class
QTEST_MAIN(MainWindowTest)
#include "test_mainwindow.moc"
