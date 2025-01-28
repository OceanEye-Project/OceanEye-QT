#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include <iostream>

NewProjectDialog::NewProjectDialog(std::shared_ptr<Project>& project)
    : QDialog()
    , ui(new Ui::NewProjectDialog)
    , currentProject(project)
{
    ui->setupUi(this);

    setModal(true);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    connect(ui->addAnnotationClass, &QPushButton::clicked, this, [this](){
        QListWidgetItem* item = new QListWidgetItem("New Item");
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->annotationClassList->addItem(item);
    });

    connect(ui->removeAnnotationClass, &QPushButton::clicked, this, [this](){
        ui->annotationClassList->takeItem(ui->annotationClassList->currentRow());
    });

    connect(ui->createProject, &QPushButton::clicked, this, &NewProjectDialog::createProject);

    connect(ui->loadAnnotationClasses, &QPushButton::clicked, this, [this](){
        auto fileName = QFileDialog::getOpenFileName(this, "Open Model", QDir::homePath(), "ONNX Model (*.onnx)");

        qInfo() << "Model loaded: Loading classes from model";

        auto classes = YOLOv8::loadClasses(fileName.toStdString());
        std::cout << "Classes: " << classes.size() << std::endl;
        ui->annotationClassList->clear();

        for (auto& className : classes) {
            std::cout << "Class: " << className.toStdString() << std::endl;
            QListWidgetItem* item = new QListWidgetItem(className);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ui->annotationClassList->addItem(item);
        }
    });
}

void NewProjectDialog::createProject() {
    bool errors = false;

    if (ui->projectName->text().isEmpty()) {
        ui->projectName->setStyleSheet("border: 1px solid red");
        errors = true;
    } else {
        ui->projectName->setStyleSheet("");
    }

    if (ui->annotationClassList->count() == 0) {
        ui->annotationClassList->setStyleSheet("border: 1px solid red");
        errors = true;
    } else {
        ui->annotationClassList->setStyleSheet("");
    }

    std::set<QString> classSet {};

    for (int i=0; i<ui->annotationClassList->count(); i++) {
        auto item = ui->annotationClassList->item(i);

        if (classSet.count(item->text()) != 0 || item->text().isEmpty()) {
            item->setBackground({"red"});
            errors = true;
        } else {
            item->setBackground({});
            classSet.insert(item->text());
        }
    }

    if (errors)
        return;

    std::vector<QString> classes {classSet.begin(), classSet.end()};

    currentProject = std::make_shared<Project>(
        ui->projectFolder->text(),
        classes,
        ui->projectName->text()
    );

    accept();
}

void NewProjectDialog::show(QString projectPath, std::function<void()> callback) {
    ui->projectFolder->setText(projectPath);

    QDialog::show();
    connect(this, &QDialog::finished, this, [this, callback](int result){
        callback();
    });
}
