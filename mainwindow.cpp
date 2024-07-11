#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(std::shared_ptr<Project>& project, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentProject(project)
    , mainImage(project)
{
    ui->setupUi(this);

    ui->mainImageContainer->setLayout(new QHBoxLayout());
    ui->mainImageContainer->layout()->addWidget(&mainImage);

    connect(ui->AddMediaBtn, &QPushButton::clicked, this, &MainWindow::addMedia);
    connect(ui->imgPrevBtn, &QPushButton::clicked, this, [this]{
        currentImg--;
        updateImageUI();
    });
    connect(ui->imgNextBtn, &QPushButton::clicked, this, [this]{
        currentImg++;
        updateImageUI();
    });
    updateImageUI();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    QStringList files = QFileDialog::getOpenFileNames(this, "Select one or more files to open", "", "Images (*.png *.xpm *.jpg)");

    for (auto& file : files) {
        currentProject->media.push_back(file);
    }

    currentProject->saveMedia();

    updateImageUI();
}
