#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QComboBox>
#include <QtConcurrent/QtConcurrent>
#include <QHBoxLayout>
#include <QDialog>
#include <QProgressBar>
#include "../util/project.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ImportDialog;
}
QT_END_NAMESPACE

class ImportWaitingDialog : public QDialog
{
    Q_OBJECT
    QProgressBar* progressBar;
    QLabel* progressLabel;

public:
    explicit ImportWaitingDialog();

signals:
    void setLabelText(QString text);
};

class ImportDialog : public QDialog
{
    Q_OBJECT

    ImportWaitingDialog dialog;

    Ui::ImportDialog *ui;
    std::map<QString, std::vector<Annotation>> annotations;

    QFutureWatcher<void> watcher;
    QFuture<void> future;

public:
    std::shared_ptr<Project>& currentProject;
    explicit ImportDialog(std::shared_ptr<Project>& project, QWidget *parent = nullptr);

public slots:
    void doImport();

signals:
    void doneImport();
};

#endif // IMPORTDIALOG_H
