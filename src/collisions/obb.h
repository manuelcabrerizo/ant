#pragma once

#include <common.h>
#include <math/vector3.h>

class OBB
{
public:
    Vector3 c;
    Vector3 u[3];
    Vector3 e;
};