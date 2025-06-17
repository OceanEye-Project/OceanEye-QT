#include "modeltrainer.h"


ModelTrainer::ModelTrainer(std::shared_ptr<Project>& project)
    : QObject{}
    , currentProject(project)
    , dialog()
    , python_logger(dialog.log_window)
{
    // connect(&watcher, &QFutureWatcher<void>::finished, this, [this]{
    //     qInfo() << "Python Finished";
    //     dialog.hide();

    //     QMessageBox msgBox;
    //     msgBox.setText("Training Finished");
    //     msgBox.exec();
    // });
}

void ModelTrainer::startTraining(TrainArgs& trainArgs) {
    dialog.show();

    // std::function<void()> trainWithProject = [this, trainArgs]() {
    //     return train(
    //     currentProject->projectPath.toStdString(), trainArgs);
    // };

    // future = QtConcurrent::run(trainWithProject);
    // watcher.setFuture(future);
    python_process = new QProcess(this);
    connect(python_process, &QProcess::readyReadStandardOutput, this, [this]() {
        QString output = python_process->readAllStandardOutput();
        dialog.log_window->appendMessage(output);
    });

    connect(python_process, &QProcess::readyReadStandardError, this, [this]() {
        QString output = python_process->readAllStandardError();
        dialog.log_window->appendMessage(output);
    });

    connect(python_process, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            dialog.log_window->appendMessage("Training completed successfully.");
        } else {
            dialog.log_window->appendMessage("Training failed with exit code: " + exitCode);
        }
        // dialog.hide();
    });

    QString pythonPath = QCoreApplication::applicationDirPath() + "/python/python.exe";
    QString scriptPath = QCoreApplication::applicationDirPath() + "/python_scripts/train.py";

    QStringList arguments;
    arguments << "--project_path" << currentProject->projectPath
                << "--model" << QString::fromStdString(trainArgs.model)
                << "--save_file_path" << trainArgs.saveFilePath
                << "--time" << QString::number(trainArgs.time)
                << "--epochs" << QString::number(trainArgs.epochs)
                << "--patience" << QString::number(trainArgs.patience);

    python_process->start(pythonPath, QStringList() << scriptPath << arguments);
}

// PYBIND11_EMBEDDED_MODULE(embeded_logger, module)
// {
//     py::class_<PythonLogger>(module, "PythonLogger")
//     .def("write", &PythonLogger::write)
//         .def("flush", &PythonLogger::flush);
// }

void ModelTrainer::setup_python_env() {
    std::filesystem::path exeDir = QCoreApplication::applicationDirPath().toStdString();

    // py::module os = py::module::import("os");
    // os.attr("chdir")((exeDir / "python").string());

    // py::module sys = py::module::import("sys");
    // sys.attr("path").attr("append")(exeDir.string());

    // py::module::import("embeded_logger");
    // py::module::import("sys").attr("stdout") = python_logger;

    // py::module installer = py::module::import("install_packages");
    // installer.attr("setup")(exeDir.string());
}

void ModelTrainer::train(std::string project_path, TrainArgs trainArgs) {
    // {
    //     py::scoped_interpreter guard{};

    //     try {
    //         setup_python_env();

    //         py::print("========== New Training Session ==========");
    //         py::print("Checking dependancies");

    //         py::module installer = py::module::import("install_packages");

    //         installer.attr("ensure_pip")();
    //         installer.attr("install_dependencies")();

    //     } catch (py::error_already_set & e) {
    //         std::cout << e.what() << std::endl;
    //     }
    // }


    // {
    //     py::scoped_interpreter guard{};

    //     try {
    //         setup_python_env();

    //         py::print("Running Checks");

    //         py::module trainer = py::module::import("train");
    //         trainer.attr("run_checks")();

    //         py::print("Starting Training");

    //         py::list pythonClasses;

    //         for (const auto& c : currentProject->classes) {
    //             pythonClasses.append(c.toStdString());
    //         }
            
    //         py::dict pythonTrainArgs;
    //         pythonTrainArgs["classes"] = pythonClasses;
    //         pythonTrainArgs["model"] = trainArgs.model;
    //         pythonTrainArgs["patience"] = trainArgs.patience;

    //         if (trainArgs.epochs > 0) pythonTrainArgs["epochs"] = trainArgs.epochs;
    //         else pythonTrainArgs["epochs"] = 5000;

    //         if (trainArgs.time > 0) pythonTrainArgs["time"] = trainArgs.time;

    //         py::object model = trainer.attr("train")(project_path, pythonTrainArgs);

    //         py::object py_onnx_path = model.attr("export")(
    //             "format"_a="onnx"
    //         );

    //         std::string onnx_path = py_onnx_path.cast<std::string>();

    //         QFile::rename(
    //             QString::fromStdString(onnx_path),
    //             trainArgs.saveFilePath
    //         );

    //     } catch (py::error_already_set & e) {
    //         std::cout << e.what() << std::endl;
    //     }
    // }

}
