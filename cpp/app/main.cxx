#include <CppLib/point.h>
#include <CppLib/triangle.h>
#include <iostream>

void doPoint() {
    std::cout << "Start point source..." << std::endl;

    PointSource algo;
    auto field = algo.run({0, 1, 2, 3, 10, 20, 7, 6, 3});

    for (const double &i : field) {
        std::cout << i << std::endl;
    }

    std::cout << "...End" << std::endl;
}

void doTriangle() {
    std::cout << "Start triangle source..." << std::endl;

    // The source
    // Burgers vector = 1 micron displacement in x direction
    // Material properties
    //   mu = 30e9: Shear modulus (Pa)
    //   nu = 0.25: Poisson's ratio
    TriangleSource source(Point3D{0.0, 0.0, 0.0}, Point3D{1.0, 0.0, 0.0},
                          Point3D{0.0, 1.0, 0.0}, Vec3{1e-6, 0.0, 0.0}, 1e9,
                          0.25, 6);

    // Compute stress tensor
    auto field = source.run({0, 1, 2, 3, 10, 20, 7, 6, 3});
    for (const double &i : field) {
        std::cout << i << std::endl;
    }

    std::cout << "...End" << std::endl;
}

int main(int argc, char **argv) {
    doPoint();
    doTriangle();
    return 0;
}
