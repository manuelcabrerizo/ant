#pragma once

#include <common.h>
#include <math/vector3.h>

class Cylinder
{
public:
    void Init(Vector3 p, Vector3 q, f32 r);

    Vector3 p;
    Vector3 q;
    f32 r;
};