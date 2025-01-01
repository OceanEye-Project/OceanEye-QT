#include "python.h"
#include <QCoreApplication>
#include <iostream>
#include <filesystem>

void install_packages() {
    std::filesystem::path path = QCoreApplication::applicationDirPath().toStdString();
    path /= "python";

    py::scoped_interpreter guard{}; // start the interpreter and keep it alive

    try {
        py::module os = py::module::import("os");
        os.attr("chdir")(path.string());

        py::module sys = py::module::import("sys");
        sys.attr("path").attr("append")(path.string());
        // set sys.argv with path to avoid error
        // sys.attr("argv") = py::list({path.string()});

        py::module script = py::module::import("install_packages");
        py::object result = script.attr("main")();

    } catch (py::error_already_set & e) {
        std::cout << e.what() << std::endl;
    }
}

int run_python() {
    std::filesystem::path exeDir = QCoreApplication::applicationDirPath().toStdString();

    // auto pythonHome = (exeDir / "lib").string();
    // auto appHome = (exeDir / "lib").string()

    //Py_OptimizeFlag = 1;
    //Py_SetProgramName(L"OceanEye Helper");
    //Py_SetPath(pythonHome.c_str());
    //Py_SetPythonHome(std::wstring(pythonHome).c_str());

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

        py::module installer = py::module::import("install_packages");
        installer.attr("setup")();
        //installer.attr("get_pip")();
        //installer.attr("install")("ultralytics");

        py::exec(R"(
            print("Installation Finished")
        )");

        py::module trainer = py::module::import("train");
        trainer.attr("run_checks")();
        // trainer.attr("train")();

    } catch (py::error_already_set & e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
