#include <CppLib/algos.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

PYBIND11_MODULE(pyalgo, m)
{
    pybind11::class_<math::GreenFunction>(m, "GreenFunction")
        .def(pybind11::init<>()) // ctor
        .def(pybind11::init<const Vec3&, const Vec3&>()) // ctor
        .def("run", &math::GreenFunction::run);
}
