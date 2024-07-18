// Executable command: ./build/OceanEyeTest.app/Contents/MacOS/OceanEyeTest
#include <QtTest/QtTest>
#include "../mainwindow.h"

class MainWindowTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void mainWindowTest();
};

// Define the test functions
void WelcomeWindowTest::initTestCase() {
    // Initialization code here
}

void mainWindowTest(){
    // code here
}

// Register the test class
QTEST_MAIN(MainWindowTest)
#include "test_mainwindow.moc"
