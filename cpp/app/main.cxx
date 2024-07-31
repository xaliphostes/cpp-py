#include <iostream>
#include <CppLib/algos.h>

int main() {
    std::cerr << "Start..." << std::endl ;

    math::Algo algo;
    auto field = algo.run({0, 1, 2, 3, 10, 20});

    for (const auto &i : field ) {
        std::cerr << i << std::endl ;
    }

    std::cerr << "...End" << std::endl ;

    return 0;
}
