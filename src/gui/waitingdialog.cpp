#include "waitingdialog.h"


WaitingDialog::WaitingDialog(const QString &labelText)
    : QDialog()
{
    const auto layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(labelText));

    progress->setRange(0, 0);
    layout->addWidget(progress);

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::FramelessWindowHint);
}

void WaitingDialog::setRange(int min, int max) {
    progress->setRange(min, max);
}

void WaitingDialog::updateProgress(int value) {
    currentProgress = value;
    progress->setValue(value);
}

void WaitingDialog::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Escape)
        return;

    QDialog::keyPressEvent(e);
}

void WaitingDialog::closeEvent(QCloseEvent *e) {
    e->ignore();
}

void WaitingDialog::hide() {
    prepareToClose = true;
    QDialog::hide();
}
