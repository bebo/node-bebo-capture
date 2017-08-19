#pragma once

#include "nan.h"
NAN_METHOD(getConstraints);
NAN_METHOD(setConstraints);

class ConstraintEntity
{
  public:
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t fps = 0;

    ConstraintEntity(){};

    ~ConstraintEntity()
    {
    }
};