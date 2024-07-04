#include "algos.h"
#include <algorithm>

namespace math
{
    
    Algo::Algo()
    {
    }

    Algo::Algo(const Name &name): name_(name)
    {
    }

    Algo::~Algo()
    {
    }

    DisplacementField Algo::run(const Coordinates &coordinates)
    {
        Coordinates values = coordinates; // copy

        auto fct = [](double value) { return 2 * value; }; // lambda function

        std::transform(coordinates.begin(), coordinates.end(), values.begin(), fct);

        // same as
        // for (uint32_t i = 0; i < coordinates.size(); ++i)
        // {
        //     values[i] *= 2;
        // }

        return values;
    }

}
