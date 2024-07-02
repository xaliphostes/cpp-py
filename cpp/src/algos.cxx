#include "algos.h"
#include <algorithm>

namespace math
{

    DisplacementField Algo::run(const Coordinates &coordinates)
    {
        auto fct = [](double value) { return 2 * value; };

        auto values = coordinates;
        std::transform(coordinates.begin(), coordinates.end(), values.begin(), fct);
        return values;
    }

}
