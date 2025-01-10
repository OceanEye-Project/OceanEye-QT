#include "modeltrainer.h"

ModelTrainer::ModelTrainer(std::shared_ptr<Project>& project)
    : QObject{}
    , currentProject(project)
{}

void ModelTrainer::startTraining() {
    std::filesystem::path exeDir = QCoreApplication::applicationDirPath().toStdString();

    // auto pythonHome = (exeDir / "lib").string();
    // auto appHome = (exeDir / "lib").string()

    //Py_OptimizeFlag = 1;
    //Py_SetProgramName(L"OceanEye Helper");
    //Py_SetPath(pythonHome.c_str());
    //Py_SetPythonHome(std::wstring(pythonHome).c_str());

    py::scoped_interpreter guard{}; // start the interpreter and keep it alive

    try {
        py::exec(R"(
            print("Starting Training")
        )");

        // Disable build of __pycache__ folders
        py::exec(R"(
            import sys
            sys.dont_write_bytecode = True
        )");

        py::module os = py::module::import("os");
        os.attr("chdir")((exeDir / "python").string());

        py::module sys = py::module::import("sys");
        sys.attr("path").attr("append")(exeDir.string());

        py::module installer = py::module::import("install_packages");
        installer.attr("setup")(exeDir.string());
        //installer.attr("get_pip")();
        //installer.attr("install")("ultralytics");

        py::exec(R"(
            print("Installation Finished")
        )");

        py::module trainer = py::module::import("train");
        trainer.attr("run_checks")();

        py::exec(R"(
            print("Starting Training")
        )");

        trainer.attr("train")(currentProject->projectPath.toStdString());

    } catch (py::error_already_set & e) {
        std::cout << e.what() << std::endl;
    }
}
