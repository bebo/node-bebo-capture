#pragma once

#include "nan.h"
NAN_METHOD(getConstraint);
NAN_METHOD(setConstraint);

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