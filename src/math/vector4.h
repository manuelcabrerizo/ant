#pragma once

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
    Vector4(Vector3 vector, float scalar);
};