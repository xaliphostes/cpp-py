#include "algos.h"
#include <algorithm>

namespace math
{
    Algo::Algo()
    {
        name_ = "undefined";
    }

    Algo::Algo(const Name &name)
    {
        name_ = name;
    }

    Algo::~Algo()
    {
    }

    DisplacementField Algo::run(const Coordinates &coordinates)
    {
        auto fct = [](double value) { return 2 * value; };

        Coordinates values = coordinates;

        // std::transform(coordinates.begin(), coordinates.end(), values.begin(), fct);

        for (uint32_t i = 0; i < coordinates.size(); ++i)
        {
            values[i] *= 2;
        }

        return values;
    }

}
