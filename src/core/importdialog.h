#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QDialog>
#include "../util/project.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ImportDialog;
}
QT_END_NAMESPACE

class ImportDialog : public QDialog
{
    Q_OBJECT

    Ui::ImportDialog *ui;
    std::map<QString, std::vector<Annotation>> annotations;

public:
    std::shared_ptr<Project>& currentProject;
    explicit ImportDialog(std::shared_ptr<Project>& project, QWidget *parent = nullptr);

public slots:
    void doExport();


signals:
};

#endif // IMPORTDIALOG_H
