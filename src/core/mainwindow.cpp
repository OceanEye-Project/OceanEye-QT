#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    resize(screenWidth, screenHeight);

    mainImage.annotationEditBtn = ui->annotationEditBtn;
    mainImage.annotationDeleteBtn = ui->annotationDeleteBtn;
    mainImage.annotationNewBtn = ui->annotationNewBtn;
    mainImage.annotationClassCombo = ui->annotationClassCombo;

    connect(ui->annotationEditBtn, &QPushButton::clicked, &mainImage, &AnnotatedImage::triggerRepaint);
    connect(ui->annotationNewBtn, &QPushButton::clicked, &mainImage, &AnnotatedImage::triggerRepaint);
    connect(ui->annotationDeleteBtn, &QPushButton::clicked, &mainImage, &AnnotatedImage::triggerRepaint);

    connect(&mainImage, &AnnotatedImage::annotationsChanged, this, &MainWindow::updateTable);

    ui->mainImageContainer->setLayout(new QHBoxLayout());
    ui->mainImageContainer->layout()->addWidget(&mainImage);

    ui->dataTable->setModel(model);

    connect(ui->detectBtn, &QPushButton::clicked, &detectOptions, &DetectOptions::show);
    connect(&detectOptions, &DetectOptions::runDetection, this, &MainWindow::runDetection);
    connect(&detectOptions, &DetectOptions::runSpecificDetection, this, &MainWindow::runSpecificDetection);

    connect(ui->AddMediaBtn, &QPushButton::clicked, this, &MainWindow::addMedia);
    connect(ui->loadModelBtn, &QPushButton::clicked, this, &MainWindow::loadModel);
    connect(ui->actionExport, &QAction::triggered, &exportDialog, &ExportDialog::show);
    connect(ui->actionEditMedia, &QAction::triggered, &editMediaDialog, &EditMediaDialog::show);
    connect(ui->editMediaBtn, &QPushButton::clicked, &editMediaDialog, &EditMediaDialog::show);
    connect(ui->actionOpenSettings, &QAction::triggered, &settingsDialog, &Settings::show);
    connect(ui->actionSettings, &QAction::triggered, &settingsDialog, &Settings::show);
    connect(&settingsDialog.projectSettings, &ProjectSettings::updateImageUI, this, &MainWindow::updateImageUI);

    connect(ui->imgPrevBtn, &QPushButton::clicked, this, [this]{
        // If currentImg is 0, set it to the last image
        if(currentImg < 1){
            currentImg = currentProject->media.size() - 1;
        }
        // Else reverse one image
        else{
            currentImg--;
        }

        updateImageUI();
    });
    connect(ui->imgNextBtn, &QPushButton::clicked, this, [&]{
        currentImg++;
        updateImageUI();
    });
    connect(ui->modelConfSlider, &QSlider::valueChanged, this, [project](int conf){
        project->setModelConf(conf);
    });
    connect(project.get(), &Project::modelLoaded, this, [this](QString modelPath){ ui->modelPathLabel->setText(modelPath); });

    if (project->isModelLoaded()) {
        ui->modelPathLabel->setText(QString::fromStdString(project->model->modelPath));
    }
    if (project->settings.contains("Model Confidence")) {
        int conf = project->settings.value("Model Confidence").toInt();
        ui->modelConfSlider->setValue(conf);
    } else {
        ui->modelConfSlider->setValue(70);
    }

    connect(&videoSlicer, &VideoSlicer::doneSlicing, this, &MainWindow::updateImageUI);
    connect(&editMediaDialog, &EditMediaDialog::mediaChanged, this, &MainWindow::updateImageUI);

    for (int i=0; i<model_classes.size();i++) {
        ui->annotationClassCombo->addItem(
            QString::fromStdString(model_classes.at(i)),
            QVariant(i)
            );
    }
    updateImageUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTable() {
    currentProject->setAnnotation(currentProject->media.at(currentImg), mainImage.annotations);

    model->clear();

    model->setColumnCount(6);
    model->setHorizontalHeaderItem(0, new QStandardItem("Class"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Confidence"));
    model->setHorizontalHeaderItem(2, new QStandardItem("X"));
    model->setHorizontalHeaderItem(3, new QStandardItem("Y"));
    model->setHorizontalHeaderItem(4, new QStandardItem("Width"));
    model->setHorizontalHeaderItem(5, new QStandardItem("Height"));

    for (auto& annotation : mainImage.annotations) {
        QList<QStandardItem *> items;

        items.append(new QStandardItem(QString(annotation.className)));
        items.append(new QStandardItem(QString::number(annotation.confidence)));
        items.append(new QStandardItem(QString::number(annotation.box.x())));
        items.append(new QStandardItem(QString::number(annotation.box.y())));
        items.append(new QStandardItem(QString::number(annotation.box.width())));
        items.append(new QStandardItem(QString::number(annotation.box.height())));

        model->appendRow(items);
    }

    // ui->dataTable->setModel(model);
}

void MainWindow::loadModel() {
    QString file = QFileDialog::getOpenFileName(this, "Select one or more files to open", "", "Models (*.onnx)");
    currentProject->loadModel(file);
}

void MainWindow::runDetection() {
    if (currentProject->media.empty())
        return;

    currentProject->runDetection(currentProject->media.at(currentImg));

    updateImageUI();
}

void MainWindow::runSpecificDetection(QList<QListWidgetItem *> classTypes) { 
    if (currentProject->media.empty())
        return;

    currentProject->runSpecificDetection(currentProject->media.at(currentImg), classTypes);

    updateImageUI();
}

void MainWindow::updateImageUI() {
    if (currentProject->media.empty()) {
        ui->imgPathLabel->setText("No Images Loaded");
        ui->imgCountLabel->setText("0 / 0");
        mainImage.setImage();
        currentImg = 0;
        return;
    }

    if (currentImg < 0)
        currentImg = currentProject->media.size() - 1 + (currentImg % currentProject->media.size());

    if (currentImg >= currentProject->media.size())
        currentImg = currentImg % currentProject->media.size();

    ui->imgPathLabel->setText(currentProject->media.at(currentImg));
    ui->imgCountLabel->setText(QString("%1 / %2").arg(currentImg+1).arg(currentProject->media.size()));

    mainImage.setImage(currentProject->media.at(currentImg));
}

void MainWindow::addMedia() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Select one or more files to open", "", "Image/Video (*.png *.xpm *.jpg *.mp4 *.mov *.avi *.webm)");

    QStringList videosToSlice {};

    for (auto& file : files) {
        QByteArray imageFormat = QImageReader::imageFormat(file);

        if (imageFormat == "") {
            videosToSlice.append(file);
        } else {
            currentProject->media.push_back(file);
        }

    }

    currentProject->saveMedia();

    videoSlicer.slice(videosToSlice);

    updateImageUI();
}

