#include <CppLib/algos.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

PYBIND11_MODULE(pyalgo, m)
{
    pybind11::class_<math::Algo>(m, "Algo")
        .def(pybind11::init<>()) // ctor
        .def("run", &math::Algo::run);
}
