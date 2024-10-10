#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>

MainWindow::MainWindow(std::shared_ptr<Project>& project, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentProject(project)
    , mainImage(project)
    , exportDialog(project)
    , detectOptions(project)
    , editMediaDialog(project)
    , settingsDialog(project)
    , videoSlicer(project)
{
    ui->setupUi(this);
    awesome = new fa::QtAwesome(this);
    awesome->initFontAwesome();
    ui->annotationNewBtn->setIcon(awesome->icon(fa::fa_solid, fa::fa_plus));
    
    // Set window size to match the primary screen
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    resize(screenWidth, screenHeight);

    // Set focus policy for the data table to ignore keyboard events
    ui->dataTable->setFocusPolicy(Qt::NoFocus);

    // Set up annotation buttons and class combo box
    mainImage.annotationNewBtn = ui->annotationNewBtn;
    mainImage.annotationClassCombo = ui->annotationClassCombo;
    
    // Connect annotation buttons to trigger repaint
    connect(ui->annotationNewBtn, &QPushButton::clicked, &mainImage, &AnnotatedImage::triggerRepaint);

    // Connect annotationsChanged signal to updateTable slot
    connect(&mainImage, &AnnotatedImage::annotationsChanged, this, &MainWindow::updateTable);

    // Set up main image container
    ui->mainImageContainer->setLayout(new QHBoxLayout());
    ui->mainImageContainer->layout()->addWidget(&mainImage);

    // Set model for data table
    ui->dataTable->setModel(model);

    // Connect detection-related signals and slots
    // connect(ui->detectBtn, &QPushButton::clicked, &detectOptions, &DetectOptions::show);
    connect(ui->detectBtn, &QPushButton::clicked, this, &MainWindow::runDetection);
    connect(&detectOptions, &DetectOptions::runDetection, this, &MainWindow::runDetection);
    connect(&detectOptions, &DetectOptions::runSpecificDetection, this, &MainWindow::runSpecificDetection);
    connect(&videoSlicer, &VideoSlicer::doneSlicing, this, &MainWindow::doneSlicing);

    // Connect media button
    connect(ui->AddMediaBtn, &QPushButton::clicked, this, [this]() {
        addMedia();
    });

    // Connect menu actions
    connect(ui->actionExport, &QAction::triggered, &exportDialog, &ExportDialog::show);
    connect(ui->actionEditMedia, &QAction::triggered, &editMediaDialog, &EditMediaDialog::show);
    connect(ui->editMediaBtn, &QPushButton::clicked, &editMediaDialog, &EditMediaDialog::show);
    connect(ui->actionOpenSettings, &QAction::triggered, &settingsDialog, &Settings::show);
    connect(ui->actionOpenFile, &QAction::triggered, [this]() {
        WelcomeWindow *welcomeWindow = new WelcomeWindow(currentProject);
        welcomeWindow->openProject();
    });
    connect(ui->actionSettings, &QAction::triggered, &settingsDialog, &Settings::show);
    connect(&settingsDialog.projectSettings, &ProjectSettings::updateImageUI, this, &MainWindow::updateImageUI);
    connect(&settingsDialog.projectSettings, &ProjectSettings::updateModelConfSlider, [this, project]() {
        ui->modelConfSlider->setValue(project->settings.value("Model Confidence").toInt());
    });

    // Connect the clicked signal of the data table to a custom slot
    connect(ui->dataTable, &QTableView::clicked, this, &MainWindow::handleTableClick);


    // Connect image navigation buttons
    connect(ui->imgPrevBtn, &QPushButton::clicked, this, &MainWindow::navigatePrevious);
    connect(ui->imgNextBtn, &QPushButton::clicked, this, &MainWindow::navigateNext);

    // Connect model confidence slider
    connect(ui->modelConfSlider, &QSlider::valueChanged, this, [project](int conf){
        project->setModelConf(conf);
    });
    connect(project.get(), &Project::modelLoaded, this, [this](QString modelPath){ ui->modelPathLabel->setText(modelPath); });

    // Set initial model path if loaded
    if (project->isModelLoaded()) {
        auto file_info = QFileInfo(QString::fromStdString(project->model->modelPath));

        if (file_info.fileName().length() > 30) {
            QString fileName = file_info.fileName();
            fileName.truncate(30);
            fileName.append("...");
            ui->modelPathLabel->setText(fileName);
        } else {
            ui->modelPathLabel->setText(file_info.fileName());
        }
        
        ui->modelPathLabel->setToolTip(file_info.absoluteFilePath());
    }
    
    // Set initial model confidence
    if (project->settings.contains("Model Confidence")) {
        int conf = project->settings.value("Model Confidence").toInt();
        ui->modelConfSlider->setValue(conf);
    } else {
        ui->modelConfSlider->setValue(70);
    }

    // Connect video slicer and media edit signals
    connect(&videoSlicer, &VideoSlicer::doneSlicing, this, &MainWindow::updateImageUI);
    connect(&editMediaDialog, &EditMediaDialog::mediaChanged, this, &MainWindow::updateImageUI);

    // next and previous image shortcuts
    connect(
        new QShortcut(QKeySequence(Qt::Key_Right), this), 
        &QShortcut::activated, this, &MainWindow::navigateNext
    );

    connect(
        new QShortcut(QKeySequence(Qt::Key_Left), this), 
        &QShortcut::activated, this, &MainWindow::navigatePrevious
    );

    // Populate annotation class combo box
    for (int i=0; i<model_classes.size();i++) {
        ui->annotationClassCombo->addItem(
            QString::fromStdString(model_classes.at(i)),
            QVariant(i)
            );
    }
    
    // Initial UI update
    updateImageUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// void MainWindow::keyPressEvent(QKeyEvent *event){
//     // Always handle left and right arrow keys for image navigation
//     if (event->key() == Qt::Key_Right) {
//         navigateNext();
//         event->accept();  // Stop event propagation
//     } else if (event->key() == Qt::Key_Left) {
//         navigatePrevious();
//         event->accept();  // Stop event propagation
//     } else {
//         // Let the base class handle other keys
//         QMainWindow::keyPressEvent(event);
//     }
// }

void MainWindow::navigateNext()
{
    // Move to next image and update UI
    currentImg++;
    updateImageUI();
}

void MainWindow::navigatePrevious()
{
    // Move to previous image, wrapping around to the end if at the beginning
    if (currentImg < 1) {
        currentImg = currentProject->media.size() - 1;
    } else {
        currentImg--;
    }
    updateImageUI();
}

void MainWindow::updateTable() {
    if (currentProject->media.empty())
        return;

    // Update project annotations
    currentProject->setAnnotation(currentProject->media.at(currentImg), mainImage.annotations);

    // Clear and set up the model
    model->clear();
    model->setColumnCount(2);
    model->setHorizontalHeaderItem(0, new QStandardItem("Class"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Confidence"));
    // model->setHorizontalHeaderItem(2, new QStandardItem("X"));
    // model->setHorizontalHeaderItem(3, new QStandardItem("Y"));
    // model->setHorizontalHeaderItem(4, new QStandardItem("Width"));
    // model->setHorizontalHeaderItem(5, new QStandardItem("Height"));

    // Populate the model with annotation data
    for (auto& annotation : mainImage.annotations) {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(QString(annotation.className)));
        items.append(new QStandardItem(QString::number(annotation.confidence)));
        // items.append(new QStandardItem(QString::number(annotation.box.x())));
        // items.append(new QStandardItem(QString::number(annotation.box.y())));
        // items.append(new QStandardItem(QString::number(annotation.box.width())));
        // items.append(new QStandardItem(QString::number(annotation.box.height())));
        model->appendRow(items);
    }
}

void MainWindow::loadModel(QString file) {
    // Open file dialog if no file specified
    if (file == "") 
        file = QFileDialog::getOpenFileName(this, "Select one or more files to open", "", "Models (*.onnx)");
    currentProject->loadModel(file);
}

void MainWindow::runDetection() {
    // Run detection on current image if media is available
    if (currentProject->media.empty())
        return;

    bool containsAll = false;
    for (QListWidgetItem* item : currentProject->selectedItems) {
        if (item->text() == "All") {
            containsAll = true;
        }
    }

    if(containsAll) {
        currentProject->runDetection(currentProject->media.at(currentImg));
    } else {
        currentProject->runSpecificDetection(currentProject->media.at(currentImg), currentProject->selectedItems);
    }
    updateImageUI();
}

void MainWindow::runSpecificDetection(QList<QListWidgetItem *> classTypes) { 
    // Run detection for specific classes on current image if media is available
    if (currentProject->media.empty())
        return;

    currentProject->runSpecificDetection(currentProject->media.at(currentImg), classTypes);
    updateImageUI();
}

void MainWindow::updateImageUI() {
    // Handle case when no media is loaded
    if (currentProject->media.empty()) {
        ui->imgPathLabel->setText("No Images Loaded");
        ui->imgCountLabel->setText("0 / 0");
        mainImage.setImage();
        currentImg = 0;
        return;
    }

    // Ensure currentImg is within bounds
    if (currentImg < 0)
        currentImg = currentProject->media.size() - 1 + (currentImg % currentProject->media.size());
    if (currentImg >= currentProject->media.size())
        currentImg = currentImg % currentProject->media.size();

    // Update UI elements
    ui->imgPathLabel->setText(currentProject->media.at(currentImg));
    ui->imgCountLabel->setText(QString("%1 / %2").arg(currentImg+1).arg(currentProject->media.size()));
    mainImage.setImage(currentProject->media.at(currentImg));
}

void MainWindow::addMedia(QStringList files) {
    // Open file dialog if no files specified
    if (files.isEmpty()) {
        files = QFileDialog::getOpenFileNames(this, "Select one or more files to open", "", "Image/Video (*.png *.xpm *.jpg *.mp4 *.mov *.avi *.webm)");
    }

    QStringList videosToSlice {};

    // Process each file
    for (auto& file : files) {
        QByteArray imageFormat = QImageReader::imageFormat(file);

        if (imageFormat == "") {
            // Add video files to slice
            videosToSlice.append(file);
        } else {
            // Add image files directly to project
            currentProject->media.push_back(file);
        }
    }

    // Save media to project
    currentProject->saveMedia();
    
    // Slice videos asynchronously
    QFuture<std::vector<QString>> slicedVideos = videoSlicer.slice(videosToSlice);

    // Update UI
    updateImageUI();
}

void MainWindow::doneSlicing() {
    // This method could be called when video slicing is complete
    // It's currently empty, but could be used to update UI or perform other actions
}

void MainWindow::handleTableClick(const QModelIndex &index) {
    // Check if the index is valid
    if (!index.isValid()) return;

    // Get the row of the clicked item
    int row = index.row();

    // Retrieve data from the model for the clicked row
    QString className = model->data(model->index(row, 0)).toString();  // Assuming first column is "Class"
    QString confidence = model->data(model->index(row, 1)).toString();  // Assuming second column is "Confidence"

    // Print the row data using qDebug()
    qDebug() << "Row clicked:" << row << ", Class:" << className << ", Confidence:" << confidence;
    mainImage.selectedAnnotation = row;
    mainImage.triggerRepaint();
}