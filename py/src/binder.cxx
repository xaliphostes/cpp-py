#include <CppLib/point.h>
#include <CppLib/triangle.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

PYBIND11_MODULE(pyalgo, m)
{
    pybind11::class_<PointSource>(m, "PointSource")
        .def(pybind11::init<>()) // ctor
        .def(pybind11::init<const Vec3&, const Vec3&>()) // ctor
        .def(pybind11::init<const Vec3&, const Vec3&, double, double>()) // ctor
        .def("run", &PointSource::run);

    pybind11::class_<TriangleSource>(m, "TriangleSource")
        .def(pybind11::init<const Point3D&, const Point3D&, const Point3D&, const Vec3&>()) // ctor
        .def(pybind11::init<const Point3D&, const Point3D&, const Point3D&, const Vec3&, double, double, uint8_t>()) // ctor
        .def("run", &TriangleSource::run);
}
