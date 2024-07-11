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
}

void WelcomeWindow::loadProjectFromPath(QString projectPath) {
    std::cout << projectPath.toStdString() << std::endl;

    currentProject = std::make_shared<Project>(projectPath);

    if (std::find(projects.begin(), projects.end(), projectPath) == projects.end())
        projects.push_back(projectPath);

    saveProjectPaths();

    mainWindow = new MainWindow(currentProject);
    mainWindow->setWindowTitle("OceanEye");
    mainWindow->show();
}

void WelcomeWindow::loadProjectPaths() {
    qDeleteAll(ui->projectArray->children());

    QSettings settings {"oceaneye", "oceaneye"};

    int size = settings.beginReadArray("projects");

    QVBoxLayout* projectArrayLayout = new QVBoxLayout();
    // QWidget* projectArray = new QWidget();
    ui->projectArray->setLayout(projectArrayLayout);

    for (int i = 0; i < size; ++i) {
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
    settings.setValue("size", 0);

    for (int i=0; i<projects.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("path", projects.at(i));
    }

    settings.endArray();
}
