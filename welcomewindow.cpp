#include "welcomewindow.h"
#include "./ui_welcomewindow.h"

MainWindow* mainWindow;

WelcomeWindow::WelcomeWindow(std::shared_ptr<Project>& currentProject, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WelcomeWindow)
    , currentProject(currentProject)
{
    ui->setupUi(this);

    connect(ui->openProjectBtn, &QPushButton::clicked, this, &WelcomeWindow::openProject);
    connect(ui->openProjectBtn2, &QPushButton::clicked, this, &WelcomeWindow::openProject);
    // TODO new project btn

    loadProjectPaths();

    int project_index = 0;

    if (projects.empty())
        project_index = 1;

    ui->welcomeStack->setCurrentIndex(project_index);

    connect(ui->projectBtn, &QPushButton::clicked, this, [this, project_index]{ui->welcomeStack->setCurrentIndex(project_index);});
    connect(ui->settingsBtn, &QPushButton::clicked, this, [this]{ui->welcomeStack->setCurrentIndex(2);});
    connect(ui->aboutBtn, &QPushButton::clicked, this, [this]{ui->welcomeStack->setCurrentIndex(3);});

    QSettings settings {"oceaneye", "oceaneye"};

    std::cout << settings.fileName().toStdString() << std::endl;

    QLabel* settingsPathLabel = new QLabel(settings.fileName());
    settingsPathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    ui->settingsPage->layout()->addWidget(new QLabel("Global Settings Path"));
    ui->settingsPage->layout()->addWidget(settingsPathLabel);
    ui->settingsPage->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
}

WelcomeWindow::~WelcomeWindow()
{
    delete ui;
}

void WelcomeWindow::openProject() {
    QString projectDir = QFileDialog::getExistingDirectory(this, "Open Project", "", QFileDialog::ShowDirsOnly);

    loadProjectFromPath(QFileInfo(projectDir).absoluteFilePath());
    if (!projectDir.isEmpty()) {
        loadProjectFromPath(QFileInfo(projectDir).absoluteFilePath());
        emit projectOpened(); // Emit the signal when a project is opened
    }
}

void WelcomeWindow::loadProjectFromPath(QString projectPath) {
    std::cout << projectPath.toStdString() << std::endl;

    currentProject = std::make_shared<Project>(projectPath);

    auto projectPos = std::find(projects.begin(), projects.end(), projectPath);

    if (projectPos != projects.end()) {
        projects.erase(projectPos);
    }

    projects.push_back(projectPath);

    saveProjectPaths();

    mainWindow = new MainWindow(currentProject);
    mainWindow->setWindowTitle("OceanEye");
    mainWindow->show();

    emit projectOpened();
}

void WelcomeWindow::loadProjectPaths() {
    qDeleteAll(ui->projectArray->children());

    QSettings settings {"oceaneye", "oceaneye"};

    int size = settings.beginReadArray("projects");

    QVBoxLayout* projectArrayLayout = new QVBoxLayout();

    ui->projectArray->setLayout(projectArrayLayout);

    projects.clear();

    for (int i = size - 1; i >= 0; --i) {
        settings.setArrayIndex(i);

        QString projectPath = settings.value("path").toString();
        projects.push_back(projectPath);

        QWidget* project = new QWidget();
        QHBoxLayout* projectLayout = new QHBoxLayout();
        QPushButton* projectBtn = new QPushButton(projectPath);
        QPushButton* closeBtn = new QPushButton("x");

        connect(closeBtn, &QPushButton::clicked, this, [this, i] {
            projects.erase(projects.begin() + i);
            saveProjectPaths();
            loadProjectPaths();
        });

        connect(projectBtn, &QPushButton::clicked, this, [this, projectPath] {
            loadProjectFromPath(projectPath);
        });

        project->setLayout(projectLayout);

        projectLayout->addWidget(projectBtn);
        projectLayout->addStretch();
        projectLayout->addWidget(closeBtn);

        projectArrayLayout->addWidget(project);
    }
    settings.endArray();

    projectArrayLayout->addStretch();
}

void WelcomeWindow::saveProjectPaths() {
    QSettings settings {"oceaneye", "oceaneye"};

    settings.beginWriteArray("projects");
    settings.remove("");

    for (int i=0; i<projects.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("path", projects.at(i));
    }

    settings.endArray();
}
