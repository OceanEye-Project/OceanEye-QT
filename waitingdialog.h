#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QProgressBar>

class WaitingDialog : public QDialog
{
public:
    explicit WaitingDialog(const QString &labelText);

    void hide();
    void updateProgress(int value);
    void setRange(int min, int max);

protected:

    void keyPressEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *e) override;

private:
    QProgressBar* progress = new QProgressBar();
    bool prepareToClose = false;
};
#endif // WAITINGDIALOG_H

