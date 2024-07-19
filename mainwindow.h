#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QImageReader>
#include <QProgressBar>

#include "opencv2/opencv.hpp"
#include "project.h"
#include "annotatedimage.h"
#include "exportdialog.h"
#include "editmediadialog.h"
#include "videoslicer.h"

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

    friend class MainWindowTest;

private:
    std::shared_ptr<Project>& currentProject;
    int currentImg {};
    AnnotatedImage mainImage;
    QStandardItemModel* model {new QStandardItemModel()};
    ExportDialog exportDialog;
    EditMediaDialog editMediaDialog;
    VideoSlicer videoSlicer;

    Ui::MainWindow *ui;
    void updateImageUI();
    void addMedia();
    void loadModel();
    void runDetection();

public slots:
    void updateTable();

};
#endif // MAINWINDOW_H