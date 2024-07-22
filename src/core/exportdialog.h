#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QDialog>
#include "../util/project.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ExportDialog;
}
QT_END_NAMESPACE

class ExportDialog : public QDialog
{
    Q_OBJECT

    Ui::ExportDialog *ui;
    std::map<QString, std::vector<Annotation>> annotations;

public:
    std::shared_ptr<Project>& currentProject;
    explicit ExportDialog(std::shared_ptr<Project>& project, QWidget *parent = nullptr);

public slots:
    void doExport();


signals:
};

#endif // EXPORTDIALOG_H
