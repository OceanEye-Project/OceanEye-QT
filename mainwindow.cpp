#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(std::shared_ptr<Project>& project, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentProject(project)
    , mainImage(project)
    , exportDialog(project)
    , videoSlicer(project)
{
    ui->setupUi(this);

    connect(&mainImage, &AnnotatedImage::annotationsChanged, this, &MainWindow::updateTable);

    ui->mainImageContainer->setLayout(new QHBoxLayout());
    ui->mainImageContainer->layout()->addWidget(&mainImage);

    ui->dataTable->setModel(model);

    connect(ui->AddMediaBtn, &QPushButton::clicked, this, &MainWindow::addMedia);
    connect(ui->detectBtn, &QPushButton::clicked, this, &MainWindow::runDetection);
    connect(ui->loadModelBtn, &QPushButton::clicked, this, &MainWindow::loadModel);
    connect(ui->actionExport, &QAction::triggered, this, [this]{exportDialog.show();});

    connect(ui->imgPrevBtn, &QPushButton::clicked, this, [this]{
        currentImg--;
        updateImageUI();
    });
    connect(ui->imgNextBtn, &QPushButton::clicked, this, [this]{
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
    if (project->settings.contains("modelConf")) {
        int conf = project->settings.value("modelConf").toInt();
        ui->modelConfSlider->setValue(conf);
    } else {
        ui->modelConfSlider->setValue(70);
    }

    connect(&videoSlicer, &VideoSlicer::doneSlicing, this, &MainWindow::updateImageUI);

    updateImageUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTable() {
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
    currentProject->runDetection(currentProject->media.at(currentImg));

    updateImageUI();
}

void MainWindow::updateImageUI() {
    if (currentProject->media.empty()) {
        ui->imgPathLabel->setText("No Images Loaded");
        ui->imgCountLabel->setText("0 / 0");
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

