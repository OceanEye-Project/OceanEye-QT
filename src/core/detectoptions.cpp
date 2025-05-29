#include "detectoptions.h"
#include "./ui_detectoptions.h"

DetectOptions::DetectOptions(std::shared_ptr<Project>& project)
    : QWidget{}
    , ui(new Ui::DetectOptions)
    , currentProject(project)
{
    ui->setupUi(this);
    setWindowTitle("Detection Options");
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    ui->classList->setSelectionMode(QAbstractItemView::MultiSelection);

    // Populate the drop down menu
    // Populate the drop down menu
    ui->classList->addItem("All");  // Add "All" item
    
    // Select the "All" item and add it to currentProject->selectedItems
    QListWidgetItem* allItem = ui->classList->findItems("All", Qt::MatchExactly).first();
    allItem->setSelected(true);  // Select "All"
    
    // Assuming currentProject has a selectedItems member (as QList<QListWidgetItem*>)
    if (currentProject) {
        currentProject->selectedItems.append(allItem);  // Add "All" to selectedItems
    }
    
    for (int i=0; i<currentProject->classes.size();i++) {
        ui->classList->addItem(
            currentProject->classes.at(i)
        );
    }

    connect(ui->cancelBtn, &QPushButton::clicked, this, &DetectOptions::close);
    connect(ui->okBtn, &QPushButton::clicked, this, [this]() {
        if(currentProject) {
            QList<QListWidgetItem*> selectedItems = ui->classList->selectedItems(); 
            currentProject->selectedItems = selectedItems;
        }
        DetectOptions::close();
    });
}
