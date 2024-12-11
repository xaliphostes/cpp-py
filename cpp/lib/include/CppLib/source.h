#pragma once
#include "types.h"

class Source {
  public:
    virtual StressField run(const Coordinates &) const = 0;
};
