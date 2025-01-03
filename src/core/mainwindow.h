#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt includes
#include <QMainWindow>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QImageReader>
#include <QProgressBar>
#include <QKeyEvent>
#include <QShortcut>
#include <QKeySequence>

// Third-party libraries
#include "opencv2/opencv.hpp"

// Custom includes
#include "../gui/annotatedimage.h"
#include "../gui/settings/settings.h"
#include "../util/project.h"
#include "exportdialog.h"
#include "importdialog.h"
#include "editmediadialog.h"
#include "detectoptions.h"
#include "../python/modeltrainer.h"
#include "../util/videoslicer.h"
#include "../util/yolov8.h"
#include "welcomewindow.h"
#include "QtAwesome.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// MainWindow class declaration
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor takes a shared pointer to a Project and an optional parent widget
    MainWindow(std::shared_ptr<Project>& project, QWidget *parent = nullptr);
    // Destructor
    ~MainWindow();

private:
    // Reference to the current project
    std::shared_ptr<Project>& currentProject;
    // Index of the currently displayed image
    int currentImg {};
    // Widget for displaying and annotating images
    AnnotatedImage mainImage;
    // Model for storing annotation data
    QStandardItemModel* model {new QStandardItemModel()};
    // Dialog for exporting project data
    ExportDialog exportDialog;
    // Dialog for importing project data
    ImportDialog importDialog;
    // Dialog for editing media files
    EditMediaDialog editMediaDialog;
    // Dialog for configuring detection options
    DetectOptions detectOptions;
    // Dialog for application settings
    Settings settingsDialog;
    // Utility for slicing video into frames
    VideoSlicer videoSlicer;
    // Utility for training models
    ModelTrainer modelTrainer;

    // Pointer to the UI components
    Ui::MainWindow *ui;
    // For Icons
    fa::QtAwesome* awesome;

    // Method for highlighting annotation when a row in the table is clicked
    void handleTableClick(const QModelIndex &index);

    // Method for adding media files to the project
    void addMedia(QStringList files = {});

    // Method for loading a machine learning model
    void loadModel(QString file = "");
    // Methods for navigating between images
    void navigateNext();
    void navigatePrevious();

    // Friend classes that need access to private members
    friend class WelcomeWindow;
    friend class MainWindowTest;
    friend class DetectOptions;

public slots:
    // Slot for updating the annotation table
    void updateTable();
    // Slot for updating the image UI
    void updateImageUI();
    // Slot for running object detection
    void runDetection();
    // Slot for running detection on specific classes
    void runSpecificDetection(QList<QListWidgetItem *> classTypes);
    // Slot called when video slicing is complete
    void doneSlicing();
};

#endif // MAINWINDOW_H
