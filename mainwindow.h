#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "project.h"
#include "annotatedimage.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<Project>& project, QWidget *parent = nullptr);
    ~MainWindow();

private:
    std::shared_ptr<Project>& currentProject;
    int currentImg {};
    AnnotatedImage mainImage;

    Ui::MainWindow *ui;
    void updateImageUI();
    void addMedia();
};
#endif // MAINWINDOW_H
