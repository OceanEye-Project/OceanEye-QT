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
    install_packages();

    std::filesystem::path path = QCoreApplication::applicationDirPath().toStdString();
    path /= "python";

    py::scoped_interpreter guard{}; // start the interpreter and keep it alive

    try {
        py::module os = py::module::import("os");
        os.attr("chdir")(path.string());

        py::module np = py::module::import("ultralytics");

        py::module train = py::module::import("train");
        py::object result = train.attr("run")();
    } catch (py::error_already_set & e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}