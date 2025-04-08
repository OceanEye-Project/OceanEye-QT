#ifndef MODELTRAINER_H
#define MODELTRAINER_H

#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>
#include <QDialog>
#include <QLabel>
#include <QFont>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QMessageBox>

#include <iostream>
#include <filesystem>

#include "../util/project.h"

#include "../gui/waitingdialog.h"

// python and QT both use the macro "slots"
#pragma push_macro("slots")
#undef slots
#include <Python.h>
#include <pybind11/embed.h>
#include <pybind11/iostream.h>
#pragma pop_macro("slots")

namespace py = pybind11;
using namespace pybind11::literals;

// TODO model progress chart / better output

struct TrainArgs {
    std::string model;
    QString saveFilePath;
    double time;
    int epochs;
    int patience;
};

// Displays python output in the GUI
class LogWindow : public QPlainTextEdit {
    Q_OBJECT
public:
    LogWindow() {
        setReadOnly(true);
        setBackgroundVisible(false);

        QFont f("monospace");
        f.setStyleHint(QFont::Monospace);
        setFont(f);

        connect(this, &LogWindow::message_written, this, &LogWindow::appendMessage);
    }

    void appendMessage(std::string str) {
        moveCursor(QTextCursor::End);
        insertPlainText(QString::fromStdString(str));
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
        QCoreApplication::processEvents();
    }

signals:
    void message_written(std::string value);
};

// Captures python output and sends to the GUI
class PythonLogger {
public:
    LogWindow* log_window;

    PythonLogger(LogWindow* log_window) : log_window(log_window) {}

    void write(std::string str){
        std::cout << str;
        emit log_window->message_written(str);
    }

    void flush() {
        //std::cout << std::flush;
    }
};

// The popup for model training
class PythonDialog : public QDialog {
    Q_OBJECT
public:
    LogWindow* log_window;
    bool prepareToClose = false;

    PythonDialog() {
        setWindowTitle("Training Output");

        const auto layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("Training Output"));

        log_window = new LogWindow();
        layout->addWidget(log_window);

        setAttribute(Qt::WA_DeleteOnClose);
        setWindowModality(Qt::ApplicationModal);
        //setWindowFlags(Qt::FramelessWindowHint);
    }
    void keyPressEvent(QKeyEvent *e) {
        if (e->key() == Qt::Key_Escape)
            return;

        QDialog::keyPressEvent(e);
    }

    void closeEvent(QCloseEvent *e) {
        e->ignore();
    }

    void hide() {
        prepareToClose = true;
        QDialog::hide();
    }
};

class ModelTrainer : public QObject
{
    Q_OBJECT
    PythonDialog dialog;

    QFutureWatcher<void> watcher;
    QFuture<void> future;

    PythonLogger python_logger;

    std::shared_ptr<Project>& currentProject;

    void setup_python_env();
    void train(std::string project_path, TrainArgs trainArgs);

public:
    explicit ModelTrainer(std::shared_ptr<Project>& project);

    void startTraining(TrainArgs& trainArgs);
};

#endif // MODELTRAINER_H
