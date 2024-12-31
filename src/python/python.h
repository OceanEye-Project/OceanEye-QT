#ifndef PYTHON_H
#define PYTHON_H

// python and QT both use the macro "slots"
#pragma push_macro("slots")
#undef slots
#include <Python.h>
#include <pybind11/embed.h>
#pragma pop_macro("slots")

namespace py = pybind11;

int run_python();

#endif // PYTHON_H
