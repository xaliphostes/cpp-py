#pragma once
#include "types.h"

namespace math
{

    // --------------------------------------------

    class AlgoBase
    {
    public:
        virtual DisplacementField run(const Coordinates& coordinates) = 0;
    };

    // --------------------------------------------

    class Algo : public AlgoBase
    {
    public:
        DisplacementField run(const Coordinates& coordinates) override;
    };

    // --------------------------------------------

}
