#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(std::shared_ptr<Project>& project, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentProject(project)
    , mainImage(project)
    , exportDialog(project)
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

 // TODO https://www.francescmm.com/non-closable-qdialog/
    videoSliceDialog = new QDialog();
    videoSliceDialog->setWindowFlags(Qt::FramelessWindowHint);
    videoSliceDialog->setWindowModality(Qt::ApplicationModal);
    QVBoxLayout* videoSliceDialogLayout = new QVBoxLayout();
    videoSliceDialog->setLayout(videoSliceDialogLayout);

    QProgressBar* videoSliceProgress = new QProgressBar();
    videoSliceProgress->setRange(0, 0);
    videoSliceDialogLayout->addWidget(new QLabel("Slicing Video(s)..."));
    videoSliceDialogLayout->addWidget(videoSliceProgress);

    connect(&slicerWatcher, &QFutureWatcher<std::vector<QString>>::finished, this, [this]{
        std::cout << "all threads done, resuts: " << videoSliceFuture.resultCount() << std::endl;
        for (int i=0; i<videoSliceFuture.resultCount(); i++) {
            std::vector<QString> newImages = videoSliceFuture.resultAt(i);
            currentProject->media.insert(currentProject->media.end(), newImages.begin(), newImages.end());
        }
        currentProject->saveMedia();
        videoSliceDialog->hide();
        updateImageUI();
    });

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

std::vector<QString> sliceVideo (const QString& video, const QString& projectPath) {
    std::cout << "Slicing " << video.toStdString() << std::endl;

    cv::Mat frame;
    std::string framePath;

    std::vector<QString> savedFrames {};

    cv::VideoCapture cap(video.toStdString());

    if (!cap.isOpened()) {
        std::cerr << "error opening video file";
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    int skipSeconds = 5;
    int frameInterval = static_cast<int>(fps * skipSeconds);

    bool result;
    int currentFrame = 0;

    std::filesystem::path videoPath(video.toStdString());
    std::filesystem::path stillsPath(projectPath.toStdString());

    while (cap.read(frame)) {
        cap.set(cv::CAP_PROP_POS_FRAMES, currentFrame);
        currentFrame += frameInterval;

        framePath = (stillsPath / (videoPath.stem().string() + "_" + std::to_string(currentFrame) + ".jpeg")).string();

        // Check if the file already exists
        // If it doesn't, continue writing
        if (!std::filesystem::exists(framePath))
        {
            result = cv::imwrite(framePath, frame);
            if (!result) {
                std::cerr << "Failed to save frame: " << framePath << std::endl;
            } else {
                savedFrames.push_back(QString::fromStdString(framePath));
                std::cout << "Frame saved successfully: " << currentFrame << " / " << cap.get(cv::CAP_PROP_FRAME_COUNT) << " " << framePath << std::endl;
            }
        }
        else {
            std::cout << "Frame already exists: " << framePath << std::endl;
        }
            // if (result) {
            //     savedFrames.push_back(QString::fromStdString(framePath));
            // }
        // }
        // progress = (double)currentFrame / cap.get(cv::CAP_PROP_FRAME_COUNT);
    }
    std::cout << "Done slicing video" << std::endl;

    return savedFrames;
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

    if (videosToSlice.size() > 0) {
        videoSliceDialog->show();

        std::function<std::vector<QString>(const QString&)> sliceVideoWithProject = [this](const QString& video) {
            return sliceVideo(video, currentProject->projectPath);
        };

        videoSliceFuture = QtConcurrent::mapped(videosToSlice, sliceVideoWithProject);
        slicerWatcher.setFuture(videoSliceFuture);
    }

    updateImageUI();
}

