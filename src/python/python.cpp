#include "python.h"
#include <QCoreApplication>
#include <iostream>
#include <filesystem>

int run_python() {

    std::filesystem::path path = QCoreApplication::applicationDirPath().toStdString();
    path /= "python";

    py::scoped_interpreter guard{}; // start the interpreter and keep it alive

    py::module os = py::module::import("os");
    os.attr("chdir")(path.string());

    py::module train = py::module::import("train");
    py::object result = train.attr("run")();

    return 0;
}