#include "modeltrainer.h"


ModelTrainer::ModelTrainer(std::shared_ptr<Project>& project)
    : QObject{}
    , currentProject(project)
    , dialog()
    , python_logger(dialog.log_window)
{
    connect(&watcher, &QFutureWatcher<void>::finished, this, [this]{
        qInfo() << "Python Finished";
        dialog.hide();
    });
}

void ModelTrainer::startTraining() {
    dialog.show();

    std::function<void()> trainWithProject = [this]() {
        return train(currentProject->projectPath.toStdString());
    };

    future = QtConcurrent::run(trainWithProject);
    watcher.setFuture(future);
}

PYBIND11_EMBEDDED_MODULE(embeded_logger, module)
{
    py::class_<PythonLogger>(module, "PythonLogger")
    .def("write", &PythonLogger::write)
        .def("flush", &PythonLogger::flush);
}

void ModelTrainer::train(std::string project_path) {
    std::filesystem::path exeDir = QCoreApplication::applicationDirPath().toStdString();

    py::scoped_interpreter guard{}; // start the interpreter and keep it alive

    try {
        // Disable build of __pycache__ folders
        py::exec(R"(
            import sys
            sys.dont_write_bytecode = True
        )");

        py::module os = py::module::import("os");
        os.attr("chdir")((exeDir / "python").string());

        py::module sys = py::module::import("sys");
        sys.attr("path").attr("append")(exeDir.string());

        py::module::import("embeded_logger");
        py::module::import("sys").attr("stdout") = python_logger;

        py::exec(R"(
            print("Starting Training")
        )");

        py::exec(R"(
            print("Checking dependancies")
        )");

        py::module installer = py::module::import("install_packages");
        installer.attr("setup")(exeDir.string());
        installer.attr("ensure_pip")();
        installer.attr("install")("ultralytics");

        py::module trainer = py::module::import("train");
        trainer.attr("run_checks")();

        py::exec(R"(
            print("Starting Training")
        )");

        trainer.attr("train")(project_path);

    } catch (py::error_already_set & e) {
        std::cout << e.what() << std::endl;
    }
}
