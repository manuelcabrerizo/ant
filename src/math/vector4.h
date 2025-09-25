#pragma once

#include "vector3.h"

class Vector4
{
public:
    union
    {
        struct
        {
            float x, y, z, w;
        };
        float v[4];
    };

    Vector4();
    Vector4(float x, float y, float z, float w);
    Vector4(Vector3 vector, float scalar);
};